/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2015 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "regionSplit.H"
#include "cyclicPolyPatch.H"
#include "processorPolyPatch.H"
#include "globalIndex.H"
#include "syncTools.H"
#include "FaceCellWave.H"
#include "minData.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(regionSplit, 0);

}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::regionSplit::calcNonCompactRegionSplit
(
    const globalIndex& globalFaces,
    const boolList& blockedFace,
    const List<labelPair>& explicitConnections,

    labelList& cellRegion
) const
{
    // Field on cells and faces.
    List<minData> cellData(mesh().nCells());
    List<minData> faceData(mesh().nFaces());

    // Take over blockedFaces by seeding a negative number
    // (so is always less than the decomposition)
    label nUnblocked = 0;
    forAll(faceData, faceI)
    {
        if (blockedFace.size() && blockedFace[faceI])
        {
            faceData[faceI] = minData(-2);
        }
        else
        {
            nUnblocked++;
        }
    }

    // Seed unblocked faces
    labelList seedFaces(nUnblocked);
    List<minData> seedData(nUnblocked);
    nUnblocked = 0;


    forAll(faceData, faceI)
    {
        if (blockedFace.empty() || !blockedFace[faceI])
        {
            seedFaces[nUnblocked] = faceI;
            // Seed face with globally unique number
            seedData[nUnblocked] = minData(globalFaces.toGlobal(faceI));
            nUnblocked++;
        }
    }


    // Propagate information inwards
    FaceCellWave<minData> deltaCalc
    (
        mesh(),
        explicitConnections,
        false,  // disable walking through cyclicAMI for backwards compatibility
        seedFaces,
        seedData,
        faceData,
        cellData,
        mesh().globalData().nTotalCells()+1
    );


    // And extract
    cellRegion.setSize(mesh().nCells());
    forAll(cellRegion, cellI)
    {
        if (cellData[cellI].valid(deltaCalc.data()))
        {
            cellRegion[cellI] = cellData[cellI].data();
        }
        else
        {
            // Unvisited cell -> only possible if surrounded by blocked faces.
            // If so make up region from any of the faces
            const cell& cFaces = mesh().cells()[cellI];
            label faceI = cFaces[0];

            if (blockedFace.size() && !blockedFace[faceI])
            {
                FatalErrorIn("regionSplit::calcNonCompactRegionSplit(..)")
                    << "Problem: unblocked face " << faceI
                    << " at " << mesh().faceCentres()[faceI]
                    << " on unassigned cell " << cellI
                    << mesh().cellCentres()[faceI]
                    << exit(FatalError);
            }
            cellRegion[cellI] = globalFaces.toGlobal(faceI);
        }
    }
}


Foam::autoPtr<Foam::globalIndex> Foam::regionSplit::calcRegionSplit
(
    const bool doGlobalRegions,
    const boolList& blockedFace,
    const List<labelPair>& explicitConnections,

    labelList& cellRegion
) const
{
    // See header in regionSplit.H


    if (!doGlobalRegions)
    {
        // Block all parallel faces to avoid comms across
        boolList coupledOrBlockedFace(blockedFace);
        const polyBoundaryMesh& pbm = mesh().boundaryMesh();

        if (coupledOrBlockedFace.size())
        {
            forAll(pbm, patchI)
            {
                const polyPatch& pp = pbm[patchI];
                if (isA<processorPolyPatch>(pp))
                {
                    label faceI = pp.start();
                    forAll(pp, i)
                    {
                        coupledOrBlockedFace[faceI++] = true;
                    }
                }
            }
        }

        // Create dummy (local only) globalIndex
        labelList offsets(Pstream::nProcs()+1, 0);
        for (label i = Pstream::myProcNo()+1; i < offsets.size(); i++)
        {
            offsets[i] = mesh().nFaces();
        }
        const globalIndex globalRegions(offsets.xfer());

        // Minimise regions across connected cells
        // Note: still uses global decisions so all processors are running
        //       in lock-step, i.e. slowest determines overall time.
        //       To avoid this we could switch off Pstream::parRun.
        calcNonCompactRegionSplit
        (
            globalRegions,
            coupledOrBlockedFace,
            explicitConnections,
            cellRegion
        );

        // Compact
        Map<label> globalToCompact(mesh().nCells()/8);
        forAll(cellRegion, cellI)
        {
            label region = cellRegion[cellI];

            label globalRegion;

            Map<label>::const_iterator fnd = globalToCompact.find(region);
            if (fnd == globalToCompact.end())
            {
                globalRegion = globalRegions.toGlobal(globalToCompact.size());
                globalToCompact.insert(region, globalRegion);
            }
            else
            {
                globalRegion = fnd();
            }
            cellRegion[cellI] = globalRegion;
        }


        // Return globalIndex with size = localSize and all regions local
        labelList compactOffsets(Pstream::nProcs()+1, 0);
        for (label i = Pstream::myProcNo()+1; i < compactOffsets.size(); i++)
        {
            compactOffsets[i] = globalToCompact.size();
        }

        return autoPtr<globalIndex>(new globalIndex(compactOffsets.xfer()));
    }



    // Initial global region numbers
    const globalIndex globalRegions(mesh().nFaces());

    // Minimise regions across connected cells (including parallel)
    calcNonCompactRegionSplit
    (
        globalRegions,
        blockedFace,
        explicitConnections,
        cellRegion
    );


    // Now our cellRegion will have
    // - non-local regions (i.e. originating from other processors)
    // - non-compact locally originating regions
    // so we'll need to compact

    // 4a: count per originating processor the number of regions
    labelList nOriginating(Pstream::nProcs(), 0);
    {
        labelHashSet haveRegion(mesh().nCells()/8);

        forAll(cellRegion, cellI)
        {
            label region = cellRegion[cellI];

            // Count originating processor. Use isLocal as efficiency since
            // most cells are locally originating.
            if (globalRegions.isLocal(region))
            {
                if (haveRegion.insert(region))
                {
                    nOriginating[Pstream::myProcNo()]++;
                }
            }
            else
            {
                label procI = globalRegions.whichProcID(region);
                if (haveRegion.insert(region))
                {
                    nOriginating[procI]++;
                }
            }
        }
    }

    if (debug)
    {
        Pout<< "Counted " << nOriginating[Pstream::myProcNo()]
            << " local regions." << endl;
    }


    // Global numbering for compacted local regions
    autoPtr<globalIndex> globalCompactPtr
    (
        new globalIndex(nOriginating[Pstream::myProcNo()])
    );
    const globalIndex& globalCompact = globalCompactPtr();


    // 4b: renumber
    // Renumber into compact indices. Note that since we've already made
    // all regions global we now need a Map to store the compacting information
    // instead of a labelList - otherwise we could have used a straight
    // labelList.

    // Local compaction map
    Map<label> globalToCompact(2*nOriginating[Pstream::myProcNo()]);
    // Remote regions we want the compact number for
    List<labelHashSet> nonLocal(Pstream::nProcs());
    forAll(nonLocal, procI)
    {
        if (procI != Pstream::myProcNo())
        {
            nonLocal[procI].resize(2*nOriginating[procI]);
        }
    }

    forAll(cellRegion, cellI)
    {
        label region = cellRegion[cellI];
        if (globalRegions.isLocal(region))
        {
            // Insert new compact region (if not yet present)
            globalToCompact.insert
            (
                region,
                globalCompact.toGlobal(globalToCompact.size())
            );
        }
        else
        {
            nonLocal[globalRegions.whichProcID(region)].insert(region);
        }
    }


    // Now we have all the local regions compacted. Now we need to get the
    // non-local ones from the processors to whom they are local.
    // Convert the nonLocal (labelHashSets) to labelLists.

    labelListList sendNonLocal(Pstream::nProcs());
    forAll(sendNonLocal, procI)
    {
        sendNonLocal[procI] = nonLocal[procI].toc();
    }

    if (debug)
    {
        forAll(sendNonLocal, procI)
        {
            Pout<< "    from processor " << procI
                << " want " << sendNonLocal[procI].size()
                << " region numbers."
                << endl;
        }
        Pout<< endl;
    }


    // Get the wanted region labels into recvNonLocal
    labelListList recvNonLocal(Pstream::nProcs());
    labelListList sizes;
    Pstream::exchange<labelList, label>
    (
        sendNonLocal,
        recvNonLocal,
        sizes
    );

    // Now we have the wanted compact region labels that procI wants in
    // recvNonLocal[procI]. Construct corresponding list of compact
    // region labels to send back.

    labelListList sendWantedLocal(Pstream::nProcs());
    forAll(recvNonLocal, procI)
    {
        const labelList& nonLocal = recvNonLocal[procI];
        sendWantedLocal[procI].setSize(nonLocal.size());

        forAll(nonLocal, i)
        {
            sendWantedLocal[procI][i] = globalToCompact[nonLocal[i]];
        }
    }


    // Send back (into recvNonLocal)
    recvNonLocal.clear();
    recvNonLocal.setSize(sendWantedLocal.size());
    sizes.clear();
    Pstream::exchange<labelList, label>
    (
        sendWantedLocal,
        recvNonLocal,
        sizes
    );
    sendWantedLocal.clear();

    // Now recvNonLocal contains for every element in setNonLocal the
    // corresponding compact number. Insert these into the local compaction
    // map.

    forAll(recvNonLocal, procI)
    {
        const labelList& wantedRegions = sendNonLocal[procI];
        const labelList& compactRegions = recvNonLocal[procI];

        forAll(wantedRegions, i)
        {
            globalToCompact.insert(wantedRegions[i], compactRegions[i]);
        }
    }

    // Finally renumber the regions
    forAll(cellRegion, cellI)
    {
        cellRegion[cellI] = globalToCompact[cellRegion[cellI]];
    }

    return globalCompactPtr;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::regionSplit::regionSplit(const polyMesh& mesh, const bool doGlobalRegions)
:
    MeshObject<polyMesh, Foam::TopologicalMeshObject, regionSplit>(mesh),
    labelList(mesh.nCells(), -1)
{
    globalNumberingPtr_ = calcRegionSplit
    (
        doGlobalRegions,    //do global regions
        boolList(0, false), //blockedFaces
        List<labelPair>(0), //explicitConnections,
        *this
    );
}


Foam::regionSplit::regionSplit
(
    const polyMesh& mesh,
    const boolList& blockedFace,
    const bool doGlobalRegions
)
:
    MeshObject<polyMesh, Foam::TopologicalMeshObject, regionSplit>(mesh),
    labelList(mesh.nCells(), -1)
{
    globalNumberingPtr_ = calcRegionSplit
    (
        doGlobalRegions,
        blockedFace,        //blockedFaces
        List<labelPair>(0), //explicitConnections,
        *this
    );
}


Foam::regionSplit::regionSplit
(
    const polyMesh& mesh,
    const boolList& blockedFace,
    const List<labelPair>& explicitConnections,
    const bool doGlobalRegions
)
:
    MeshObject<polyMesh, Foam::TopologicalMeshObject, regionSplit>(mesh),
    labelList(mesh.nCells(), -1)
{
    globalNumberingPtr_ = calcRegionSplit
    (
        doGlobalRegions,
        blockedFace,            //blockedFaces
        explicitConnections,    //explicitConnections,
        *this
    );
}


// ************************************************************************* //
