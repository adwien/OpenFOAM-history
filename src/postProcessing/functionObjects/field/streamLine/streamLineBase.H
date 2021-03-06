/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2015 OpenFOAM Foundation
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

Class
    Foam::streamLineBase

SeeAlso
    Foam::streamLine
    Foam::wallBoundedStreamLine

SourceFiles
    streamLineBase.C

\*---------------------------------------------------------------------------*/

#ifndef streamLineBase_H
#define streamLineBase_H

#include "functionObjectState.H"
#include "DynamicList.H"
#include "scalarList.H"
#include "vectorList.H"
#include "writer.H"
#include "indirectPrimitivePatch.H"
#include "interpolation.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes
class objectRegistry;
class dictionary;
class mapPolyMesh;
class meshSearch;
class sampledSet;

/*---------------------------------------------------------------------------*\
                         Class streamLineBase Declaration
\*---------------------------------------------------------------------------*/

class streamLineBase
:
    public functionObjectState
{
protected:

        //- Input dictionary
        dictionary dict_;

        //- Database this class is registered to
        const objectRegistry& obr_;

        //- Load fields from files (not from objectRegistry)
        bool loadFromFiles_;

        //- Switch to send output to Info as well as to file
        Switch log_;

        //- List of fields to sample
        wordList fields_;

        //- Field to transport particle with
        word UName_;

        //- Interpolation scheme to use
        word interpolationScheme_;

        //- Whether to use +u or -u
        bool trackForward_;

        //- Maximum lifetime (= number of cells) of particle
        label lifeTime_;

        //- Track length
        scalar trackLength_;

        //- Optional trimming of tracks
        boundBox bounds_;

        //- Optional specified name of particles
        word cloudName_;

        //- Type of seed
        word seedSet_;

        //- Names of scalar fields
        wordList scalarNames_;

        //- Names of vector fields
        wordList vectorNames_;


        // Demand driven

            //- Mesh searching enigne
            autoPtr<meshSearch> meshSearchPtr_;

            //- Seed set engine
            autoPtr<sampledSet> sampledSetPtr_;

            //- Axis of the sampled points to output
            word sampledSetAxis_;

            //- File writer for scalar data
            autoPtr<writer<scalar> > scalarFormatterPtr_;

            //- File writer for vector data
            autoPtr<writer<vector> > vectorFormatterPtr_;


        // Generated data

            //- All tracks. Per track the points it passed through
            DynamicList<List<point> > allTracks_;

            //- Per scalarField, per track, the sampled values
            List<DynamicList<scalarList> > allScalars_;

            //- Per vectorField, per track, the sampled values
            List<DynamicList<vectorList> > allVectors_;


        //- Construct patch out of all wall patch faces
        autoPtr<indirectPrimitivePatch> wallPatch() const;

        //- Initialise fields, interpolators and track storage
        void initInterpolations
        (
            const label nSeeds,
            label& UIndex,
            PtrList<volScalarField>& vsFlds,
            PtrList<interpolation<scalar> >& vsInterp,
            PtrList<volVectorField>& vvFlds,
            PtrList<interpolation<vector> >& vvInterp
        );

        //- Generate point and values by interpolating from existing values
        void storePoint
        (
            const label trackI,

            const scalar w,
            const label leftI,
            const label rightI,

            DynamicList<point>& newTrack,
            DynamicList<List<scalar> >& newScalars,
            DynamicList<List<vector> >& newVectors
        ) const;

        //- Trim and possibly split a track
        void trimToBox
        (
            const treeBoundBox& bb,
            const label trackI,
            PtrList<DynamicList<point> >& newTracks,
            PtrList<DynamicList<scalarList> >& newScalars,
            PtrList<DynamicList<vectorList> >& newVectors
        ) const;

        //- Trim tracks to bounding box
        void trimToBox(const treeBoundBox& bb);

        //- Do the actual tracking to fill the track data
        virtual void track() = 0;


public:

    //- Runtime type information
    TypeName("streamLineBase");


    // Constructors

        //- Construct for given objectRegistry and dictionary.
        //  Allow the possibility to load fields from files
        streamLineBase
        (
            const word& name,
            const objectRegistry&,
            const dictionary&,
            const bool loadFromFiles = false
        );


    //- Destructor
    virtual ~streamLineBase();


    // Member Functions

        //- Read the field average data
        virtual void read(const dictionary&);

        //- Execute the averaging
        virtual void execute();

        //- Execute the averaging at the final time-loop, currently does nothing
        virtual void end();

        //- Called when time was set at the end of the Time::operator++
        virtual void timeSet();

        //- Track and write
        virtual void write();

        //- Update for changes of mesh
        virtual void updateMesh(const mapPolyMesh&);

        //- Update for mesh point-motion
        virtual void movePoints(const polyMesh&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
