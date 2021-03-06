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

#include "minMaxCondition.H"
#include "addToRunTimeSelectionTable.H"
#include "fieldTypes.H"

// * * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * //

template<>
void Foam::minMaxCondition::setValue<Foam::scalar>
(
    const word& valueType,
    const word& fieldName,
    scalar& value
) const
{
    state_.getObjectResult(functionObjectName_, fieldName, value);
}


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(minMaxCondition, 0);
    addToRunTimeSelectionTable(runTimeCondition, minMaxCondition, dictionary);

    template<>
    const char* NamedEnum<minMaxCondition::modeType, 2>::names[] =
    {
        "minimum",
        "maximum"
    };
}

const Foam::NamedEnum<Foam::minMaxCondition::modeType, 2>
    Foam::minMaxCondition::modeTypeNames_;


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::minMaxCondition::minMaxCondition
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    functionObjectState& state
)
:
    runTimeCondition(name, obr, dict, state),
    functionObjectName_(dict.lookup("functionObjectName")),
    mode_(modeTypeNames_.read(dict.lookup("mode"))),
    fieldNames_(dict.lookup("fields")),
    value_(readScalar(dict.lookup("value")))
{}


// * * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * //

Foam::minMaxCondition::~minMaxCondition()
{}


// * * * * * * * * * * * * * * Public Member Functions * * * * * * * * * * * //

bool Foam::minMaxCondition::apply()
{
    bool satisfied = true;

    if (!active_)
    {
        return satisfied;
    }

    forAll(fieldNames_, fieldI)
    {
        const word& fieldName = fieldNames_[fieldI];

        const word valueType =
            state_.objectResultType(functionObjectName_, fieldName);

        if (valueType == word::null)
        {
            WarningIn("bool Foam::minMaxCondition::apply()")
                << "Unable to find entry " << fieldName
                << " for function object " << functionObjectName_
                << ".  Condition will not be applied."
                << endl;

            continue;
        }

        scalar v = 0;
        setValue<scalar>(valueType, fieldName, v);
        setValue<vector>(valueType, fieldName, v);
        setValue<sphericalTensor>(valueType, fieldName, v);
        setValue<symmTensor>(valueType, fieldName, v);
        setValue<tensor>(valueType, fieldName, v);

        Switch ok = false;
        switch (mode_)
        {
            case mdMin:
            {
                if (v < value_)
                {
                    ok = true;
                }
                break;
            }
            case mdMax:
            {
                if (v > value_)
                {
                    ok = true;
                }
                break;
            }
        }

        Info(log_)
            << "    " << type() << ": " << modeTypeNames_[mode_] << " "
            << fieldName << ": value = " << v
            << ", threshold value = " << value_
            << ", satisfied = " << ok << endl;

        satisfied = satisfied && ok;
    }

    return satisfied;
}


void Foam::minMaxCondition::write()
{
    // do nothing
}


// ************************************************************************* //
