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

#include "averageCondition.H"
#include "addToRunTimeSelectionTable.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(averageCondition, 0);
    addToRunTimeSelectionTable(runTimeCondition, averageCondition, dictionary);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::averageCondition::averageCondition
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    functionObjectState& state
)
:
    runTimeCondition(name, obr, dict, state),
    functionObjectName_(dict.lookup("functionObjectName")),
    fieldNames_(dict.lookup("fields")),
    tolerance_(readScalar(dict.lookup("tolerance"))),
    window_(dict.lookupOrDefault<scalar>("window", -1)),
    totalTime_(fieldNames_.size(), obr_.time().deltaTValue()),
    resetOnRestart_(false)
{
    if (resetOnRestart_)
    {
        const dictionary& dict = conditionDict();

        forAll(fieldNames_, fieldI)
        {
            const word& fieldName = fieldNames_[fieldI];

            if (dict.found(fieldName))
            {
                const dictionary& valueDict = dict.subDict(fieldName);
                totalTime_[fieldI] = readScalar(valueDict.lookup("totalTime"));
            }
        }
    }
}


// * * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * //

Foam::averageCondition::~averageCondition()
{}


// * * * * * * * * * * * * * * Public Member Functions * * * * * * * * * * * //

bool Foam::averageCondition::apply()
{
    bool satisfied = true;

    if (!active_)
    {
        return satisfied;
    }

    scalar dt = obr_.time().deltaTValue();

    Info(log_)<< "    " << type() << ": " << name_ << " averages:" << nl;

    DynamicList<label> unprocessedFields(fieldNames_.size());

    forAll(fieldNames_, fieldI)
    {
        const word& fieldName(fieldNames_[fieldI]);

        scalar Dt = totalTime_[fieldI];
        scalar alpha = (Dt - dt)/Dt;
        scalar beta = dt/Dt;

        if (window_ > 0)
        {
            if (Dt - dt >= window_)
            {
                alpha = (window_ - dt)/window_;
                beta = dt/window_;
            }
            else
            {
                // Ensure that averaging is performed over window time
                // before condition can be satisfied
                satisfied = false;
            }
        }

        bool processed = false;
        calc<scalar>(fieldName, alpha, beta, satisfied, processed);
        calc<vector>(fieldName, alpha, beta, satisfied, processed);
        calc<sphericalTensor>(fieldName, alpha, beta, satisfied, processed);
        calc<symmTensor>(fieldName, alpha, beta, satisfied, processed);
        calc<tensor>(fieldName, alpha, beta, satisfied, processed);

        if (!processed)
        {
            unprocessedFields.append(fieldI);
        }

        totalTime_[fieldI] += dt;
    }

    if (unprocessedFields.size())
    {
        WarningIn("bool Foam::averageCondition::apply()")
            << "From function object: " << functionObjectName_ << nl
            << "Unprocessed fields:" << nl;

        forAll(unprocessedFields, i)
        {
            label fieldI = unprocessedFields[i];
            Info<< "        " << fieldNames_[fieldI] << nl;
        }
    }

    Info(log_) << endl;

    return satisfied;
}


void Foam::averageCondition::write()
{
    dictionary& conditionDict = this->conditionDict();

    forAll(fieldNames_, fieldI)
    {
        const word& fieldName = fieldNames_[fieldI];

        // value dictionary should be present - mean values are written there
        if (conditionDict.found(fieldName))
        {
            dictionary& valueDict = conditionDict.subDict(fieldName);
            valueDict.add("totalTime", totalTime_[fieldI], true);
        }
        else
        {
            dictionary valueDict;
            valueDict.add("totalTime", totalTime_[fieldI], true);
            conditionDict.add(fieldName, valueDict);
        }
    }
}


// ************************************************************************* //
