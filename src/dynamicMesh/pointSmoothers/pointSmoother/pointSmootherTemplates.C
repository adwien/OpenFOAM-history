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

\*---------------------------------------------------------------------------*/

#include "pointSmoother.H"
#include "syncTools.H"

// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

template <class weightType>
void Foam::pointSmoother::reset
(
    const labelList& facesToMove,
    Field<weightType>& weights,
    const bool resetInternalFaces
)
{
    autoPtr<PackedBoolList> resetPointsPtr
    (
        pointsToMove(facesToMove, resetInternalFaces)
    );

    const PackedBoolList& resetPoints(resetPointsPtr);

    forAll(resetPoints, pointI)
    {
        if (resetPoints[pointI])
        {
            weights[pointI] = pTraits<weightType>::zero;
            pointDisplacement()[pointI] = vector::zero;
        }
    }
}


template <class weightType>
void Foam::pointSmoother::average
(
    const labelList& facesToMove,
    Field<weightType>& weights
)
{
    syncTools::syncPointList
    (
        mesh(),
        weights,
        plusEqOp<weightType>(),
        pTraits<weightType>::zero
    );

    syncTools::syncPointList
    (
        mesh(),
        pointDisplacement(),
        plusEqOp<vector>(),
        vector::zero
    );

    autoPtr<PackedBoolList> averagePointsPtr
    (
        pointsToMove(facesToMove, true)
    );

    const PackedBoolList& averagePoints(averagePointsPtr);

    forAll(averagePoints, pointI)
    {
        if (averagePoints[pointI])
        {
            pointDisplacement()[pointI] /= weights[pointI];
        }
    }
}


// ************************************************************************* //
