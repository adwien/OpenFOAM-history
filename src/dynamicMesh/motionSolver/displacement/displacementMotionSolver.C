/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2012-2015 OpenFOAM Foundation
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

#include "displacementMotionSolver.H"
#include "mapPolyMesh.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(displacementMotionSolver, 0);
    defineRunTimeSelectionTable(displacementMotionSolver, displacement);
}


// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

Foam::IOobject Foam::displacementMotionSolver::points0IO(const polyMesh& mesh)
{
    const word instance =
        mesh.time().findInstance
        (
            mesh.meshDir(),
            "points0",
            IOobject::READ_IF_PRESENT
        );

    if (instance != mesh.time().constant())
    {
        // points0 written to a time folder

        return
            IOobject
            (
                "points0",
                instance,
                mesh.meshDir(),
                mesh,
                IOobject::MUST_READ,
                IOobject::NO_WRITE,
                false
            );
    }
    else
    {
        // check that points0 are actually in constant directory

        IOobject io
        (
            "points0",
            instance,
            mesh.meshDir(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE,
            false
        );

        if (io.typeHeaderOk<pointIOField>())
        {
            return io;
        }
        else
        {
            // copy of original mesh points

            return
                IOobject
                (
                    "points",
                    instance,
                    mesh.meshDir(),
                    mesh,
                    IOobject::MUST_READ,
                    IOobject::NO_WRITE,
                    false
                );
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::displacementMotionSolver::displacementMotionSolver
(
    const polyMesh& mesh,
    const IOdictionary& dict,
    const word& type
)
:
    motionSolver(mesh, dict, type),
    pointDisplacement_
    (
        IOobject
        (
            "pointDisplacement",
            time().timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        pointMesh::New(mesh)
    ),
    points0_(pointIOField(points0IO(mesh)))
{
    if (points0_.size() != mesh.nPoints())
    {
        FatalErrorIn
        (
            "displacementMotionSolver::"
            "displacementMotionSolver\n"
            "(\n"
            "    const polyMesh&,\n"
            "    const IOdictionary&,\n"
            "    const word&\n"
            ")"
        )   << "Number of points in mesh " << mesh.nPoints()
            << " differs from number of points " << points0_.size()
            << " read from file "
            <<  typeFilePath<pointIOField>
                (
                    IOobject
                    (
                        "points",
                        time().constant(),
                        mesh.meshDir(),
                        mesh,
                        IOobject::MUST_READ,
                        IOobject::NO_WRITE,
                        false
                    )
                )
            << exit(FatalError);
    }
}


Foam::displacementMotionSolver::displacementMotionSolver
(
    const polyMesh& mesh,
    const IOdictionary& dict,
    const pointVectorField& pointDisplacement,
    const pointIOField& points0,
    const word& type
)
:
    motionSolver(mesh, dict, type),
    pointDisplacement_
    (
        IOobject(pointDisplacement, "pointDisplacement"),
        pointDisplacement
    ),
    points0_(points0)
{
    if (points0_.size() != mesh.nPoints())
    {
        FatalErrorIn
        (
            "displacementMotionSolver::"
            "displacementMotionSolver\n"
            "(\n"
            "    const polyMesh&,\n"
            "    const IOdictionary&,\n"
            "    const pointVectorField&,\n"
            "    const pointIOField&,\n"
            "    const word&\n"
            ")"
        )   << "Number of points in mesh " << mesh.nPoints()
            << " differs from number of points " << points0_.size()
            << " read from file " << points0.filePath()
            << exit(FatalError);
    }
}


// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

Foam::autoPtr<Foam::displacementMotionSolver>
Foam::displacementMotionSolver::New
(
    const word& solverTypeName,
    const polyMesh& mesh,
    const IOdictionary& solverDict,
    const pointVectorField& pointDisplacement,
    const pointIOField& points0
)
{
    //const word solverTypeName(solverDict.lookup("solver"));

    Info<< "Selecting motion solver: " << solverTypeName << endl;

    const_cast<Time&>(mesh.time()).libs().open
    (
        solverDict,
        "motionSolverLibs",
        displacementConstructorTablePtr_
    );

    if (!displacementConstructorTablePtr_)
    {
        FatalErrorIn
        (
            "displacementMotionSolver::New(const polyMesh& mesh)"
        )   << "solver table is empty"
            << exit(FatalError);
    }

    displacementConstructorTable::iterator cstrIter =
        displacementConstructorTablePtr_->find(solverTypeName);

    if (cstrIter == displacementConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "displacementMotionSolver::New(const polyMesh&)"
        )   << "Unknown solver type "
            << solverTypeName << nl << nl
            << "Valid solver types are:" << endl
            << displacementConstructorTablePtr_->sortedToc()
            << exit(FatalError);
    }

    return autoPtr<displacementMotionSolver>
    (
        cstrIter()
        (
            mesh,
            solverDict,
            pointDisplacement,
            points0
        )
    );
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::displacementMotionSolver::~displacementMotionSolver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::displacementMotionSolver::movePoints(const pointField&)
{
    // No local data to update
}


void Foam::displacementMotionSolver::updateMesh(const mapPolyMesh& mpm)
{
    // pointMesh already updates pointFields

    motionSolver::updateMesh(mpm);

    // Map points0_. Bit special since we somehow have to come up with
    // a sensible points0 position for introduced points.
    // Find out scaling between points0 and current points

    // Get the new points either from the map or the mesh
    const pointField& points =
    (
        mpm.hasMotionPoints()
      ? mpm.preMotionPoints()
      : mesh().points()
    );

    // Note: boundBox does reduce
    const vector span0 = boundBox(points0_).span();
    const vector span = boundBox(points).span();

    vector scaleFactors(cmptDivide(span0, span));

    pointField newPoints0(mpm.pointMap().size());

    forAll(newPoints0, pointI)
    {
        label oldPointI = mpm.pointMap()[pointI];

        if (oldPointI >= 0)
        {
            label masterPointI = mpm.reversePointMap()[oldPointI];

            if (masterPointI == pointI)
            {
                newPoints0[pointI] = points0_[oldPointI];
            }
            else
            {
                // New point - assume motion is scaling
                newPoints0[pointI] = points0_[oldPointI] + cmptMultiply
                (
                    scaleFactors,
                    points[pointI] - points[masterPointI]
                );
            }
        }
        else
        {
            FatalErrorIn
            (
                "displacementMotionSolver::updateMesh"
                "(const mapPolyMesh&)"
            )   << "Cannot determine co-ordinates of introduced vertices."
                << " New vertex " << pointI << " at co-ordinate "
                << points[pointI] << exit(FatalError);
        }
    }

    twoDCorrectPoints(newPoints0);

    points0_.transfer(newPoints0);

    // points0 changed - set to write and check-in to database
    points0_.rename("points0");
    points0_.writeOpt() = IOobject::AUTO_WRITE;
    points0_.instance() = time().timeName();
    points0_.checkIn();
}


// ************************************************************************* //
