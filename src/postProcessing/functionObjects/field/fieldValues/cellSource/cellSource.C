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

#include "cellSource.H"
#include "fvMesh.H"
#include "volFields.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    template<>
    const char* NamedEnum<fieldValues::cellSource::sourceType, 2>::names[] =
    {
        "cellZone",
        "all"
    };


    template<>
    const char* NamedEnum<fieldValues::cellSource::operationType, 9>::names[] =
    {
        "none",
        "sum",
        "average",
        "weightedAverage",
        "volAverage",
        "volIntegrate",
        "min",
        "max",
        "CoV"
    };

    namespace fieldValues
    {
        defineTypeNameAndDebug(cellSource, 0);
        addToRunTimeSelectionTable(fieldValue, cellSource, dictionary);
    }
}


const Foam::NamedEnum<Foam::fieldValues::cellSource::sourceType, 2>
    Foam::fieldValues::cellSource::sourceTypeNames_;

const Foam::NamedEnum<Foam::fieldValues::cellSource::operationType, 9>
    Foam::fieldValues::cellSource::operationTypeNames_;


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::fieldValues::cellSource::setCellZoneCells()
{
    switch (source_)
    {
        case stCellZone:
        {
            label zoneId = mesh().cellZones().findZoneID(sourceName_);

            if (zoneId < 0)
            {
                FatalErrorIn("cellSource::cellSource::setCellZoneCells()")
                    << "Unknown cell zone name: " << sourceName_
                    << ". Valid cell zones are: " << mesh().cellZones().names()
                    << nl << exit(FatalError);
            }

            cellId_ = mesh().cellZones()[zoneId];
            nCells_ = returnReduce(cellId_.size(), sumOp<label>());
            break;
        }

        case stAll:
        {
            cellId_ = identity(mesh().nCells());
            nCells_ = returnReduce(cellId_.size(), sumOp<label>());
            break;
        }

        default:
        {
            FatalErrorIn("cellSource::setCellZoneCells()")
               << "Unknown source type. Valid source types are:"
                << sourceTypeNames_ << nl << exit(FatalError);
        }
    }

    if (debug)
    {
        Pout<< "Selected source size = " << cellId_.size() << endl;
    }
}


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

void Foam::fieldValues::cellSource::initialise(const dictionary& dict)
{
    setCellZoneCells();

    if (nCells_ == 0)
    {
        WarningIn
        (
            "Foam::fieldValues::cellSource::initialise(const dictionary&)"
        )
            << type() << " " << name_ << ": "
            << sourceTypeNames_[source_] << "(" << sourceName_ << "):" << nl
            << "    Source has no cells - deactivating" << endl;

        active_ = false;
        return;
    }

    Info(log_)
        << type() << " " << name_ << ":"
        << sourceTypeNames_[source_] << "(" << sourceName_ << "):" << nl
        << "    total cells  = " << nCells_ << nl
        << "    total volume = " << gSum(filterField(mesh().V()))
        << nl << endl;

    if (dict.readIfPresent("weightField", weightFieldName_))
    {
        Info(log_)<< "    weight field = " << weightFieldName_;
    }

    Info(log_)<< nl << endl;
}


void Foam::fieldValues::cellSource::writeFileHeader(Ostream& os) const
{
    writeHeaderValue(os, "Source", sourceTypeNames_[source_]);
    writeHeaderValue(os, "Name", sourceName_);
    writeHeaderValue(os, "Cells", nCells_);
    writeHeaderValue(os, "Operation", operationTypeNames_[operation_]);
    writeHeaderValue(os, "Scale factor", scaleFactor_);
    writeCommented(os, "Time");
    writeTabbed(os, "sum(V)");

    forAll(fields_, fieldI)
    {
        writeTabbed(os, fields_[fieldI]);
    }

    os << endl;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fieldValues::cellSource::cellSource
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
:
    fieldValue(name, obr, dict, typeName, loadFromFiles),
    source_(sourceTypeNames_.read(dict.lookup("source"))),
    operation_(operationTypeNames_.read(dict.lookup("operation"))),
    nCells_(0),
    cellId_(),
    weightFieldName_("none")
{
    if (active_)
    {
        read(dict);
        writeFileHeader(file());
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::fieldValues::cellSource::~cellSource()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::fieldValues::cellSource::read(const dictionary& dict)
{
    if (active_)
    {
        fieldValue::read(dict);

        // No additional info to read
        initialise(dict);
    }
}


void Foam::fieldValues::cellSource::write()
{
    fieldValue::write();

    if (active_)
    {
        scalar totalVolume = gSum(filterField(mesh().V()));
        file() << obr_.time().value() << tab << totalVolume;

        // Construct weight field. Note: zero size means weight = 1
        scalarField weightField;
        if (weightFieldName_ != "none")
        {
            weightField = getFieldValues<scalar>(weightFieldName_, true);
        }

        forAll(fields_, i)
        {
            const word& fieldName = fields_[i];
            bool ok = false;

            ok = ok || writeValues<scalar>(fieldName, weightField);
            ok = ok || writeValues<vector>(fieldName, weightField);
            ok = ok || writeValues<sphericalTensor>(fieldName, weightField);
            ok = ok || writeValues<symmTensor>(fieldName, weightField);
            ok = ok || writeValues<tensor>(fieldName, weightField);

            if (!ok)
            {
                WarningIn("void Foam::fieldValues::cellSource::write()")
                    << "Requested field " << fieldName
                    << " not found in database and not processed"
                    << endl;
            }
        }

        file()<< endl;

        Info(log_)<< endl;
    }
}


// ************************************************************************* //
