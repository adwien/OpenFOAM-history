/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2013-2015 OpenFOAM Foundation
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

#include "wallShearStress.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "turbulentTransportModel.H"
#include "turbulentFluidThermoModel.H"
#include "wallPolyPatch.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
defineTypeNameAndDebug(wallShearStress, 0);
}


// * * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * //

void Foam::wallShearStress::writeFileHeader(Ostream& os) const
{
    // Add headers to output data
    writeHeader(os, "Wall shear stress");
    writeCommented(os, "Time");
    writeTabbed(os, "patch");
    writeTabbed(os, "min");
    writeTabbed(os, "max");
    os << endl;
}


void Foam::wallShearStress::calcShearStress
(
    const fvMesh& mesh,
    const volSymmTensorField& Reff,
    volVectorField& shearStress
)
{
    forAllConstIter(labelHashSet, patchSet_, iter)
    {
        label patchI = iter.key();
        const polyPatch& pp = mesh.boundaryMesh()[patchI];

        vectorField& ssp = shearStress.boundaryField()[patchI];
        const vectorField& Sfp = mesh.Sf().boundaryField()[patchI];
        const scalarField& magSfp = mesh.magSf().boundaryField()[patchI];
        const symmTensorField& Reffp = Reff.boundaryField()[patchI];

        ssp = (-Sfp/magSfp) & Reffp;

        vector minSsp = gMin(ssp);
        vector maxSsp = gMax(ssp);

        file() << mesh.time().value()
            << token::TAB << pp.name()
            << token::TAB << minSsp
            << token::TAB << maxSsp
            << endl;

        Info(log_)
            << "    min/max(" << pp.name() << ") = "
            << minSsp << ", " << maxSsp << endl;
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::wallShearStress::wallShearStress
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
:
    functionObjectFile(obr, name, typeName, dict),
    name_(name),
    obr_(obr),
    active_(true),
    resultName_(name),
    log_(true),
    patchSet_()
{
    // Check if the available mesh is an fvMesh, otherwise deactivate
    if (!isA<fvMesh>(obr_))
    {
        active_ = false;
        WarningIn
        (
            "wallShearStress::wallShearStress"
            "("
                "const word&, "
                "const objectRegistry&, "
                "const dictionary&, "
                "const bool"
            ")"
        )   << "No fvMesh available, deactivating " << name_ << nl
            << endl;
    }

    read(dict);

    if (active_)
    {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);

        volVectorField* wallShearStressPtr
        (
            new volVectorField
            (
                IOobject
                (
                    resultName_,
                    mesh.time().timeName(),
                    mesh,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                mesh,
                dimensionedVector
                (
                    "0",
                    sqr(dimLength)/sqr(dimTime),
                    vector::zero
                )
            )
        );

        mesh.objectRegistry::store(wallShearStressPtr);

        writeFileHeader(file());
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::wallShearStress::~wallShearStress()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::wallShearStress::read(const dictionary& dict)
{
    if (active_)
    {
        functionObjectFile::read(dict);

        log_.readIfPresent("log", dict);
        dict.readIfPresent("resultName", resultName_);

        const fvMesh& mesh = refCast<const fvMesh>(obr_);
        const polyBoundaryMesh& pbm = mesh.boundaryMesh();

        patchSet_ =
            mesh.boundaryMesh().patchSet
            (
                wordReList(dict.lookupOrDefault("patches", wordReList()))
            );

        Info(log_)<< type() << " " << name_ << " output:" << nl;

        if (patchSet_.empty())
        {
            forAll(pbm, patchI)
            {
                if (isA<wallPolyPatch>(pbm[patchI]))
                {
                    patchSet_.insert(patchI);
                }
            }

            Info(log_)<< "    processing all wall patches" << nl << endl;
        }
        else
        {
            Info(log_)<< "    processing wall patches: " << nl;
            labelHashSet filteredPatchSet;
            forAllConstIter(labelHashSet, patchSet_, iter)
            {
                label patchI = iter.key();
                if (isA<wallPolyPatch>(pbm[patchI]))
                {
                    filteredPatchSet.insert(patchI);
                    Info(log_)<< "        " << pbm[patchI].name() << endl;
                }
                else
                {
                    WarningIn("void wallShearStress::read(const dictionary&)")
                        << "Requested wall shear stress on non-wall boundary "
                        << "type patch: " << pbm[patchI].name() << endl;
                }
            }

            Info(log_)<< endl;

            patchSet_ = filteredPatchSet;
        }
    }
}


void Foam::wallShearStress::execute()
{
    typedef compressible::turbulenceModel cmpModel;
    typedef incompressible::turbulenceModel icoModel;

    if (active_)
    {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);

        volVectorField& wallShearStress =
            const_cast<volVectorField&>
            (
                mesh.lookupObject<volVectorField>(resultName_)
            );

        Info(log_)<< type() << " " << name_ << " output:" << nl;


        tmp<volSymmTensorField> Reff;
        if (mesh.foundObject<cmpModel>(turbulenceModel::propertiesName))
        {
            const cmpModel& model =
                mesh.lookupObject<cmpModel>(turbulenceModel::propertiesName);

            Reff = model.devRhoReff();
        }
        else if (mesh.foundObject<icoModel>(turbulenceModel::propertiesName))
        {
            const icoModel& model =
                mesh.lookupObject<icoModel>(turbulenceModel::propertiesName);

            Reff = model.devReff();
        }
        else
        {
            FatalErrorIn("void Foam::wallShearStress::write()")
                << "Unable to find turbulence model in the "
                << "database" << exit(FatalError);
        }

        calcShearStress(mesh, Reff(), wallShearStress);

        Info(log_)<< endl;
    }
}


void Foam::wallShearStress::end()
{
    if (active_)
    {
        execute();
    }
}


void Foam::wallShearStress::timeSet()
{
    // Do nothing
}


void Foam::wallShearStress::write()
{
    if (active_)
    {
        const volVectorField& wallShearStress =
            obr_.lookupObject<volVectorField>(resultName_);

        Info(log_)
            << type() << " " << name_ << " output:" << nl
            << "    writing field " << wallShearStress.name() << nl
            << endl;

        wallShearStress.write();
    }
}


// ************************************************************************* //
