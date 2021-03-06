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

#include "diffusionMulticomponent.H"
#include "fvcGrad.H"
#include "fvmSup.H"
#include "reactingMixture.H"
#include "surfaceInterpolate.H"
#include "fvCFD.H"
#include "fvcVolumeIntegrate.H"

// * * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * //

template<class CombThermoType, class ThermoType>
void Foam::combustionModels::
diffusionMulticomponent<CombThermoType, ThermoType>::init()
{
    // Load default values
    if (this->coeffs().found("Ci"))
    {
        this->coeffs().lookup("Ci") >> Ci_;
    }

    if (this->coeffs().found("YoxStream"))
    {
        this->coeffs().lookup("YoxStream") >> YoxStream_;
    }

    if (this->coeffs().found("YfStream"))
    {
        this->coeffs().lookup("YfStream") >> YfStream_;
    }

    if (this->coeffs().found("sigma"))
    {
        this->coeffs().lookup("sigma") >> sigma_;
    }

    if (this->coeffs().found("ftCorr"))
    {
        this->coeffs().lookup("ftCorr") >> ftCorr_;
    }

    if (this->coeffs().found("alpha"))
    {
        alpha_ = readScalar(this->coeffs().lookup("alpha"));
    }

    if (this->coeffs().found("laminarIgn"))
    {
        this->coeffs().lookup("laminarIgn") >> laminarIgn_;
    }

    typedef typename Reaction<ThermoType>::specieCoeffs specieCoeffs;

    const speciesTable& species = this->thermo().composition().species();

    scalarList specieStoichCoeffs(species.size());
    const label nReactions = reactions_.size();

    for (label k=0; k < nReactions; k++)
    {
        RijPtr_.set
        (
            k,
            new volScalarField
            (
                IOobject
                (
                    "Rijk" + name(k),
                    this->mesh_.time().timeName(),
                    this->mesh_,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE,
                    false
                ),
                this->mesh_,
                dimensionedScalar("Rij", dimMass/dimTime/dimVolume, 0.0),
                zeroGradientFvPatchScalarField::typeName
            )
        );

        RijPtr_[k].storePrevIter();

        const List<specieCoeffs>& lhs = reactions_[k].lhs();
        const List<specieCoeffs>& rhs = reactions_[k].rhs();

        const label fuelIndex = species[fuelNames_[k]];
        const label oxydantIndex = species[oxydantNames_[k]];

        const scalar Wu = specieThermo_[fuelIndex].W();
        const scalar Wox = specieThermo_[oxydantIndex].W();

        forAll(lhs, i)
        {
            const label specieI = lhs[i].index;
            specieStoichCoeffs[specieI] = -lhs[i].stoichCoeff;
            qFuel_[k] +=
                specieThermo_[specieI].hc()*lhs[i].stoichCoeff/Wu;
        }

        forAll(rhs, i)
        {
            const label specieI = rhs[i].index;
            specieStoichCoeffs[specieI] = rhs[i].stoichCoeff;
            qFuel_[k] -=
                specieThermo_[specieI].hc()*rhs[i].stoichCoeff/Wu;
        }

        Info << "Fuel heat of combustion :" << qFuel_[k] << endl;

        s_[k] =
            (Wox*mag(specieStoichCoeffs[oxydantIndex]))
          / (Wu*mag(specieStoichCoeffs[fuelIndex]));

        Info << "stoichiometric oxygen-fuel ratio :" << s_[k] << endl;

        stoicRatio_[k] = s_[k]*YfStream_[k]/YoxStream_[k];

        Info << "stoichiometric air-fuel ratio :" << stoicRatio_[k] << endl;

        const scalar fStoich = 1.0/(1.0 + stoicRatio_[k]);

        Info << "stoichiometric mixture fraction :" << fStoich << endl;

    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CombThermoType, class ThermoType>
Foam::combustionModels::diffusionMulticomponent<CombThermoType, ThermoType>::
diffusionMulticomponent
(
    const word& modelType, const fvMesh& mesh, const word& phaseName
)
:
    CombThermoType(modelType, mesh, phaseName),
    reactions_
    (
        dynamic_cast<const reactingMixture<ThermoType>&>(this->thermo())
    ),
    specieThermo_
    (
        dynamic_cast<const reactingMixture<ThermoType>&>
            (this->thermo()).speciesData()
    ),
    RijPtr_(reactions_.size()),
    Ci_(reactions_.size(), 1.0),
    fuelNames_(this->coeffs().lookup("fuels")),
    oxydantNames_(this->coeffs().lookup("oxydants")),
    qFuel_(reactions_.size()),
    stoicRatio_(reactions_.size()),
    s_(reactions_.size()),
    YoxStream_(reactions_.size(), 0.23),
    YfStream_(reactions_.size(), 1.0),
    sigma_(reactions_.size(), 0.02),
    oxydantRes_(this->coeffs().lookup("oxydantRes")),
    ftCorr_(reactions_.size(), 0.0),
    alpha_(1),
    laminarIgn_(true)
{
    init();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CombThermoType, class ThermoType>
Foam::combustionModels::diffusionMulticomponent<CombThermoType, ThermoType>::
~diffusionMulticomponent()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

template<class CombThermoType, class ThermoType>
Foam::tmp<Foam::volScalarField> Foam::combustionModels::
diffusionMulticomponent<CombThermoType, ThermoType>::tc() const
{
    return this->chemistryPtr_->tc();
}


template<class CombThermoType, class ThermoType>
void Foam::combustionModels::
diffusionMulticomponent<CombThermoType, ThermoType>::correct()
{
    if (this->active())
    {
        typedef typename Reaction<ThermoType>::specieCoeffs specieCoeffs;
        const speciesTable& species = this->thermo().composition().species();

        const label nReactions = reactions_.size();

        PtrList<volScalarField> RijlPtr(nReactions);

        for (label k=0; k < nReactions; k++)
        {

            RijlPtr.set
            (
                k,
                new volScalarField
                (
                    IOobject
                    (
                        "Rijl" + word(k),
                        this->mesh_.time().timeName(),
                        this->mesh_,
                        IOobject::NO_READ,
                        IOobject::NO_WRITE,
                        false
                    ),
                    this->mesh_,
                    dimensionedScalar("Rijl", dimMass/dimTime/dimVolume, 0.0),
                    zeroGradientFvPatchScalarField::typeName
                )
            );

            volScalarField& Rijl = RijlPtr[k];

            // Obtain individual reaction rates for each reaction
            const label fuelIndex = species[fuelNames_[k]];

            if (laminarIgn_)
            {
                Rijl.dimensionedInternalField() =
                    -this->chemistryPtr_->calculateRR(k, fuelIndex);
            }


             // Look for the fuelStoic
            const List<specieCoeffs>& rhs = reactions_[k].rhs();
            const List<specieCoeffs>& lhs = reactions_[k].lhs();

            // Set to zero RR's
            forAll (lhs, l)
            {
                const label lIndex = lhs[l].index;
                this->chemistryPtr_->RR(lIndex) =
                    dimensionedScalar("zero", dimMass/dimTime/dimVolume, 0.0);
            }

            forAll (rhs, l)
            {
                const label rIndex = rhs[l].index;
                this->chemistryPtr_->RR(rIndex) =
                    dimensionedScalar("zero", dimMass/dimTime/dimVolume, 0.0);
            }
        }


        for (label k=0; k < nReactions; k++)
        {
            const label fuelIndex = species[fuelNames_[k]];
            const label oxydantIndex = species[oxydantNames_[k]];

            const volScalarField& Yfuel =
                this->thermo().composition().Y(fuelIndex);

            const volScalarField& Yox =
                this->thermo().composition().Y(oxydantIndex);

            const volScalarField ft
            (
                "ft" + name(k),
                (
                    s_[k]*Yfuel
                  - (Yox - YoxStream_[k])
                )
                /
                (
                    s_[k]*YfStream_[k] + YoxStream_[k]
                )
            );

            const scalar sigma = sigma_[k];

            const scalar fStoich = 1.0/(1.0 + stoicRatio_[k]) + ftCorr_[k];

            const volScalarField OAvailScaled
            (
                "OAvailScaled",
                Yox/max(oxydantRes_[k], 1e-3)
            );

            const volScalarField preExp
            (
                "preExp" + name(k),
                 1.0  + sqr(OAvailScaled)
            );

            const volScalarField prob
            (
                "prob" + name(k),
                 preExp*(1.0/(sigma*sqrt(2.0*3.1415))) *
                 exp(-sqr(ft - fStoich)/(2*sqr(sigma)))
            );

            const volScalarField RijkDiff
            (
               "RijkDiff",
                Ci_[k]*this->turbulence().muEff()*prob*
                (
                    mag(fvc::grad(Yfuel) & fvc::grad(Yox))
                )
               *pos(Yox)*pos(Yfuel)
            );

            volScalarField& Rijk = RijPtr_[k];

            if (laminarIgn_)
            {
                Rijk = min(RijkDiff, RijlPtr[k]);
            }
            else
            {
                Rijk = RijkDiff;
            }

            Rijk.relax(alpha_);

            if (this->mesh_.time().outputTime())
            {
                Rijk.write();
            }

            // Look for the fuelStoic
            const List<specieCoeffs>& rhs = reactions_[k].rhs();
            const List<specieCoeffs>& lhs = reactions_[k].lhs();

            scalar fuelStoic = 1.0;
            forAll (lhs, l)
            {
                const label lIndex = lhs[l].index;
                if (lIndex == fuelIndex)
                {
                    fuelStoic = lhs[l].stoichCoeff;
                    break;
                }
            }

            const scalar MwFuel = specieThermo_[fuelIndex].W();

            // Update left hand side species
            forAll (lhs, l)
            {
                const label lIndex = lhs[l].index;

                const scalar stoichCoeff = lhs[l].stoichCoeff;

                this->chemistryPtr_->RR(lIndex) +=
                    -Rijk*stoichCoeff*specieThermo_[lIndex].W()
                   /fuelStoic/MwFuel;

            }

            // Update right hand side species
            forAll (rhs, r)
            {
                const label rIndex = rhs[r].index;

                const scalar stoichCoeff = rhs[r].stoichCoeff;

                this->chemistryPtr_->RR(rIndex) +=
                    Rijk*stoichCoeff*specieThermo_[rIndex].W()
                    /fuelStoic/MwFuel;
            }
        }
    }
}


template<class CombThermoType, class ThermoType>
Foam::tmp<Foam::fvScalarMatrix>
Foam::combustionModels::diffusionMulticomponent<CombThermoType, ThermoType>::
R(volScalarField& Y) const
{
    tmp<fvScalarMatrix> tSu(new fvScalarMatrix(Y, dimMass/dimTime));

    fvScalarMatrix& Su = tSu();

    if (this->active())
    {
        const label specieI = this->thermo().composition().species()[Y.name()];
        Su += this->chemistryPtr_->RR(specieI);
    }

    return tSu;
}


template<class CombThermoType, class ThermoType>
Foam::tmp<Foam::volScalarField>
Foam::combustionModels::diffusionMulticomponent<CombThermoType, ThermoType>::
dQ() const
{
    tmp<volScalarField> tdQ
    (
        new volScalarField
        (
            IOobject
            (
                "dQ",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false
            ),
            this->mesh(),
            dimensionedScalar("dQ", dimEnergy/dimTime, 0.0),
            zeroGradientFvPatchScalarField::typeName
        )
    );

    if (this->active())
    {
        volScalarField& dQ = tdQ();
        dQ = this->chemistryPtr_->dQ();
    }

    return tdQ;
}


template<class CombThermoType, class ThermoType>
Foam::tmp<Foam::volScalarField>
Foam::combustionModels::diffusionMulticomponent<CombThermoType, ThermoType>::
Sh() const
{
    tmp<volScalarField> tSh
    (
        new volScalarField
        (
            IOobject
            (
                "Sh",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false
            ),
            this->mesh(),
            dimensionedScalar("zero", dimEnergy/dimTime/dimVolume, 0.0),
            zeroGradientFvPatchScalarField::typeName
        )
    );

    if (this->active())
    {
        scalarField& Sh = tSh();
        Sh = this->chemistryPtr_->Sh();
    }

    return tSh;
}


template<class CombThermoType, class ThermoType>
bool Foam::combustionModels::
diffusionMulticomponent<CombThermoType, ThermoType>::read()
{
    if (CombThermoType::read())
    {
        this->coeffs().readIfPresent("Ci", Ci_);
        this->coeffs().readIfPresent("sigma", sigma_);
        this->coeffs().readIfPresent("oxydantRes", oxydantRes_);
        this->coeffs().readIfPresent("ftCorr", ftCorr_);
        this->coeffs().readIfPresent("alpha", alpha_);
        this->coeffs().readIfPresent("laminarIgn", laminarIgn_);
        return true;
    }
    else
    {
        return false;
    }
}


// ************************************************************************* //
