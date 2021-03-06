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

#include "equationMaxIterCondition.H"
#include "addToRunTimeSelectionTable.H"
#include "fvMesh.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(equationMaxIterCondition, 0);
    addToRunTimeSelectionTable
    (
        runTimeCondition,
        equationMaxIterCondition,
        dictionary
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::equationMaxIterCondition::equationMaxIterCondition
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    functionObjectState& state
)
:
    runTimeCondition(name, obr, dict, state),
    fieldNames_(dict.lookup("fields")),
    threshold_(readLabel(dict.lookup("threshold"))),
    startIter_(dict.lookupOrDefault("startIter", 2))
{
    if (!fieldNames_.size())
    {
        WarningIn
        (
            "Foam::equationMaxIterCondition::"
            "equationMaxIterCondition"
            "("
                "const word&, "
                "const objectRegistry&, "
                "const dictionary&, "
                "functionObjectState&"
            ")"
        )
            << "No fields supplied: deactivating" << endl;

        active_ = false;
    }

    startIter_ = max(startIter_, 2);
}


// * * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * //

Foam::equationMaxIterCondition::~equationMaxIterCondition()
{}


// * * * * * * * * * * * * * * Public Member Functions * * * * * * * * * * * //

bool Foam::equationMaxIterCondition::apply()
{
    bool satisfied = false;

    if (!active_)
    {
        return true;
    }

    if (obr_.time().timeIndex() < startIter_)
    {
        // Do not start checking until start iter
        return false;
    }

    const fvMesh& mesh = refCast<const fvMesh>(obr_);
    const dictionary& solverDict = mesh.solverPerformanceDict();

    List<label> result(fieldNames_.size(), -1);

    forAll(fieldNames_, fieldI)
    {
        const word& fieldName = fieldNames_[fieldI];

        if (solverDict.found(fieldName))
        {
            const List<solverPerformance> sp(solverDict.lookup(fieldName));
            const label nIterations = sp.first().nIterations();
            result[fieldI] = nIterations;

            if (nIterations > threshold_)
            {
                satisfied = true;
            }
        }
    }

    bool valid = false;
    forAll(result, i)
    {
        if (result[i] < 0)
        {
            WarningIn("bool Foam::equationMaxIterCondition::apply()")
                << "Number of iterations data not found for field "
                << fieldNames_[i] << endl;
        }
        else
        {
            valid = true;
        }
    }

    if (!valid)
    {
        WarningIn("bool Foam::equationMaxIterCondition::apply()")
            << "Number of iterations data not found for any fields: "
            << "deactivating" << endl;

        active_ = false;
    }

    if (satisfied && valid)
    {
        Info(log_)
            << type() << ": " << name_
            << ": satisfied using threshold value: " << threshold_ << nl;

        forAll(result, resultI)
        {
            if (result[resultI] != -1)
            {
                Info(log_)
                    << "    field: " << fieldNames_[resultI]
                    << ", iterations: " << result[resultI] << nl;
            }
        }
        Info(log_)<< endl;
    }

    return satisfied;
}


void Foam::equationMaxIterCondition::write()
{
    // do nothing
}


// ************************************************************************* //
