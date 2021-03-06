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

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void Foam::fieldValues::fieldValueDelta::apply
(
    const word& resultType,
    const word& name1,
    const word& name2,
    const word& entryName1,
    const word& entryName2,
    bool& found
)
{
    if (pTraits<Type>::typeName != resultType)
    {
        return;
    }

    Type result = pTraits<Type>::zero;

    Type value1 = this->getObjectResult<Type>(name1, entryName1);
    Type value2 = this->getObjectResult<Type>(name2, entryName2);

    const word& opName = operationTypeNames_[operation_];

    switch (operation_)
    {
        case opAdd:
        {
            result = value1 + value2;
            break;
        }
        case opSubtract:
        {
            result = value1 - value2;
            break;
        }
        case opMin:
        {
            result = min(value1, value2);
            break;
        }
        case opMax:
        {
            result = max(value1, value2);
            break;
        }
        case opAverage:
        {
            result = 0.5*(value1 + value2);
            break;
        }
        default:
        {
            FatalErrorIn
            (
                "void Foam::fieldValues::fieldValueDelta::applyOperation"
                "("
                    "const word&, "
                    "const word&, "
                    "const word&, "
                    "const word&, "
                    "const word&"
                    "bool&"
                ") const"
            )
                << "Operation not supported: "
                << opName
                << abort(FatalError);
        }
    }

    const word resultName(opName + '(' + entryName1 + ',' + entryName2 + ')'
    );

    Info(log_)<< "    " << resultName << " = " << result << endl;

    this->file()<< tab << result;

    // Write state/results information
    this->setResult(resultName, result);

    found = true;
}


// ************************************************************************* //
