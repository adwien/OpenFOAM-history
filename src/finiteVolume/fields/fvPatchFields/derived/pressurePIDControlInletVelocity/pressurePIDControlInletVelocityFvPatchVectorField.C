/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2015 OpenCFD Ltd.
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

#include "pressurePIDControlInletVelocityFvPatchVectorField.H"
#include "volFields.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "surfaceFields.H"
#include "linear.H"
#include "steadyStateDdtScheme.H"

// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

const Foam::surfaceScalarField&
Foam::pressurePIDControlInletVelocityFvPatchVectorField::facePressure() const
{
    const volScalarField& p(db().lookupObject<volScalarField>(pName_));

    const word pfName(pName_ + "f");

    if (!db().foundObject<surfaceScalarField>(pfName))
    {
        surfaceScalarField* pfPtr
        (
            new surfaceScalarField(pfName, linearInterpolate(p))
        );

        pfPtr->store();
    }

    surfaceScalarField& pf
    (
        const_cast<surfaceScalarField&>
        (
            db().lookupObject<surfaceScalarField>(pfName)
        )
    );

    if (!pf.upToDate(p))
    {
        pf = linearInterpolate(p);
    }

    return pf;
}


template <class Type>
void Foam::pressurePIDControlInletVelocityFvPatchVectorField::faceZoneAverage
(
    const word& name,
    const GeometricField<Type, fvsPatchField, surfaceMesh>& field,
    scalar& area,
    Type& average
) const
{
    const fvMesh& mesh(patch().boundaryMesh().mesh());

    const faceZone& zone = mesh.faceZones()[name];

    area = 0;
    average = pTraits<Type>::zero;

    forAll(zone, faceI)
    {
        const label f(zone[faceI]);

        const scalar da(mesh.magSf()[f]);

        area += da;
        average += da*field[f];
    }

    reduce(area, sumOp<scalar>());
    reduce(average, sumOp<Type>());

    average /= area;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::pressurePIDControlInletVelocityFvPatchVectorField::
pressurePIDControlInletVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchField<vector>(p, iF),
    upstreamName_(word::null),
    downstreamName_(word::null),
    deltaP_(1),
    shapeFactor_(0),
    pName_("p"),
    phiName_("phi"),
    rhoName_("none"),
    P_(0),
    I_(0),
    D_(0),
    Q_(- gSum(*this & patch().Sf())),
    error_(0),
    errorIntegral_(0),
    oldQ_(0),
    oldError_(0),
    oldErrorIntegral_(0),
    timeIndex_(db().time().timeIndex())
{}


Foam::pressurePIDControlInletVelocityFvPatchVectorField::
pressurePIDControlInletVelocityFvPatchVectorField
(
    const pressurePIDControlInletVelocityFvPatchVectorField& ptf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchField<vector>(ptf, p, iF, mapper),
    upstreamName_(ptf.upstreamName_),
    downstreamName_(ptf.downstreamName_),
    deltaP_(ptf.deltaP_),
    shapeFactor_(ptf.shapeFactor_),
    pName_(ptf.pName_),
    phiName_(ptf.phiName_),
    rhoName_(ptf.rhoName_),
    P_(ptf.P_),
    I_(ptf.I_),
    D_(ptf.D_),
    Q_(ptf.Q_),
    error_(ptf.error_),
    errorIntegral_(ptf.errorIntegral_),
    oldQ_(ptf.oldQ_),
    oldError_(ptf.oldError_),
    oldErrorIntegral_(ptf.oldErrorIntegral_),
    timeIndex_(ptf.timeIndex_)
{}


Foam::pressurePIDControlInletVelocityFvPatchVectorField::
pressurePIDControlInletVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchField<vector>(p, iF, dict),
    upstreamName_(dict.lookup("upstream")),
    downstreamName_(dict.lookup("downstream")),
    deltaP_(readScalar(dict.lookup("deltaP"))),
    shapeFactor_(dict.lookupOrDefault<scalar>("shapeFactor", 0)),
    pName_(dict.lookupOrDefault<word>("p", "p")),
    phiName_(dict.lookupOrDefault<word>("phi", "phi")),
    rhoName_(dict.lookupOrDefault<word>("rho", "none")),
    P_(readScalar(dict.lookup("P"))),
    I_(readScalar(dict.lookup("I"))),
    D_(readScalar(dict.lookup("D"))),
    Q_(- gSum(*this & patch().Sf())),
    error_(dict.lookupOrDefault<scalar>("error", 0)),
    errorIntegral_(dict.lookupOrDefault<scalar>("errorIntegral", 0)),
    oldQ_(0),
    oldError_(0),
    oldErrorIntegral_(0),
    timeIndex_(db().time().timeIndex())
{}


Foam::pressurePIDControlInletVelocityFvPatchVectorField::
pressurePIDControlInletVelocityFvPatchVectorField
(
    const pressurePIDControlInletVelocityFvPatchVectorField& ptf
)
:
    fixedValueFvPatchField<vector>(ptf),
    upstreamName_(ptf.upstreamName_),
    downstreamName_(ptf.downstreamName_),
    deltaP_(ptf.deltaP_),
    shapeFactor_(ptf.shapeFactor_),
    pName_(ptf.pName_),
    phiName_(ptf.phiName_),
    rhoName_(ptf.rhoName_),
    P_(ptf.P_),
    I_(ptf.I_),
    D_(ptf.D_),
    Q_(ptf.Q_),
    error_(ptf.error_),
    errorIntegral_(ptf.errorIntegral_),
    oldQ_(ptf.oldQ_),
    oldError_(ptf.oldError_),
    oldErrorIntegral_(ptf.oldErrorIntegral_),
    timeIndex_(ptf.timeIndex_)
{}


Foam::pressurePIDControlInletVelocityFvPatchVectorField::
pressurePIDControlInletVelocityFvPatchVectorField
(
    const pressurePIDControlInletVelocityFvPatchVectorField& ptf,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedValueFvPatchField<vector>(ptf, iF),
    upstreamName_(ptf.upstreamName_),
    downstreamName_(ptf.downstreamName_),
    deltaP_(ptf.deltaP_),
    shapeFactor_(ptf.shapeFactor_),
    pName_(ptf.pName_),
    phiName_(ptf.phiName_),
    rhoName_(ptf.rhoName_),
    P_(ptf.P_),
    I_(ptf.I_),
    D_(ptf.D_),
    Q_(ptf.Q_),
    error_(ptf.error_),
    errorIntegral_(ptf.errorIntegral_),
    oldQ_(ptf.oldQ_),
    oldError_(ptf.oldError_),
    oldErrorIntegral_(ptf.oldErrorIntegral_),
    timeIndex_(ptf.timeIndex_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::pressurePIDControlInletVelocityFvPatchVectorField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    // Get the mesh
    const fvMesh& mesh(patch().boundaryMesh().mesh());

    // Get the time step
    const scalar deltaT(db().time().deltaTValue());

    // Get the flux field
    const surfaceScalarField& phi
    (
        db().lookupObject<surfaceScalarField>(phiName_)
    );

    // Update the old-time quantities
    if (timeIndex_ != db().time().timeIndex())
    {
        timeIndex_ = db().time().timeIndex();
        oldQ_ = Q_;
        oldError_ = error_;
        oldErrorIntegral_ = errorIntegral_;
    }

    // Get the density
    scalar rho = 1;
    if (phi.dimensions() == dimVelocity*dimArea)
    {
        // do nothing ...
    }
    else if (phi.dimensions() == dimDensity*dimVelocity*dimArea)
    {
        const fvPatchField<scalar>& rhoField =
            patch().lookupPatchField<volScalarField, scalar>(rhoName_);

        rho = gSum(rhoField*patch().magSf())/gSum(patch().magSf());
    }
    else
    {
        FatalErrorIn
        (
            "void Foam::"
            "pressurePIDControlInletVelocityFvPatchVectorField::"
            "updateCoeffs()"
        )   << "The dimensions of the field " << phiName_
            << "are not recognised. The dimensions are " << phi.dimensions()
            << ". The dimensions should be either " << dimVelocity*dimArea
            << " for an incompressible case, or "
            << dimDensity*dimVelocity*dimArea << " for a compressible case."
            << exit(FatalError);
    }

    // Patch properties
    const scalar patchA = gSum(patch().magSf());
    Q_ = - gSum(*this & patch().Sf());

    // Face-zone properties (a is upstream, b is downstream)
    scalar Aa, Ab;
    vector xa, xb;
    faceZoneAverage(upstreamName_, mesh.Cf(), Aa, xa);
    faceZoneAverage(downstreamName_, mesh.Cf(), Ab, xb);
    const scalar L = mag(xa - xb);
    const scalar LbyALinear = L/(Aa - Ab)*log(Aa/Ab);
    const scalar LbyAStep = L/2*(1/Aa + 1/Ab);
    const scalar LbyA = (1 - shapeFactor_)*LbyALinear + shapeFactor_*LbyAStep;

    // Initialise the pressure drop. If the pressure field does not exist, the
    // pressure drop is assumed to be that specified. This removes the error,
    // so there is no control and the analytic inlet velocity is applied. This
    // scenario only ever going to be applicable to potentialFoam.
    scalar deltaP = deltaP_;
    if (db().foundObject<volScalarField>(pName_))
    {
        scalar pa, pb;
        faceZoneAverage(upstreamName_, facePressure(), Aa, pa);
        faceZoneAverage(downstreamName_, facePressure(), Ab, pb);
        deltaP = pa - pb;
    }
    else
    {
        WarningIn
        (
            "void Foam::pressurePIDControlInletVelocityFvPatchVectorField::"
            "updateCoeffs()"
        )   << "The pressure field name, \"pName\", is \"" << pName_ << "\", "
            << "but a field of that name was not found. The inlet velocity "
            << "will be set to an analytical value calculated from the "
            << "specified pressure drop. No PID control will be done and "
            << "transient effects will be ignored. This behaviour is designed "
            << "to be appropriate for potentialFoam solutions. If you are "
            << "getting this warning from another solver, you have probably "
            << "specified an incorrect pressure name."
            << endl << endl;
    }

    // Target and measured flow rates
    scalar QTarget, QMeasured;
    const scalar a = (1/sqr(Ab) - 1/sqr(Aa))/(2*rho);
    if (!mesh.steady() && db().foundObject<volScalarField>(pName_))
    {
        const scalar b = LbyA/deltaT;
        const scalar c = - LbyA/deltaT*oldQ_ /* - deltaP */;
        QTarget = (- b + sqrt(sqr(b) - 4*a*(c - deltaP_)))/(2*a);
        QMeasured = (- b + sqrt(sqr(b) - 4*a*(c - deltaP)))/(2*a);
    }
    else
    {
        QTarget = sqrt(deltaP_/a);
        QMeasured = sqrt(deltaP/a);
    }

    // Errors
    error_ = QTarget - QMeasured;
    errorIntegral_ = oldErrorIntegral_ + 0.5*(error_ + oldError_);
    const scalar errorDifferential = oldError_ - error_;

    // Update the field
    operator==
    (
      - patch().nf()
       *(
            QTarget
          + P_*error_
          + I_*errorIntegral_
          + D_*errorDifferential
        )/patchA
    );

    // Log output
    if (debug)
    {
        const dimensionSet pDimensions(phi.dimensions()*dimVelocity/dimArea);
        const scalar error = deltaP/deltaP_ - 1;
        const scalar newQ = - gSum(*this & patch().Sf());
        Info<< "pressurePIDControlInletVelocityFvPatchField " << patch().name()
            << endl << "         "
            << dimensionedScalar("U", dimVelocity, newQ/patchA)
            << endl << "    "
            << dimensionedScalar("deltaP", pDimensions, deltaP)
            << " (" << mag(error)*100 << "\% "
            << (error < 0 ? "below" : "above") << " the target)" << endl;
    }

    fixedValueFvPatchField<vector>::updateCoeffs();
}


void Foam::pressurePIDControlInletVelocityFvPatchVectorField::write
(
    Ostream& os
) const
{
    fvPatchField<vector>::write(os);

    os.writeKeyword("deltaP") << deltaP_ << token::END_STATEMENT << nl;
    os.writeKeyword("upstream") << upstreamName_ << token::END_STATEMENT << nl;
    os.writeKeyword("downstream")
        << downstreamName_ << token::END_STATEMENT << nl;
    os.writeKeyword("shapeFactor") << shapeFactor_
        << token::END_STATEMENT << nl;
    writeEntryIfDifferent<word>(os, "p", "p", pName_);
    writeEntryIfDifferent<word>(os, "rho", "none", rhoName_);
    os.writeKeyword("P") << P_ << token::END_STATEMENT << nl;
    os.writeKeyword("I") << I_ << token::END_STATEMENT << nl;
    os.writeKeyword("D") << D_ << token::END_STATEMENT << nl;
    os.writeKeyword("error") << error_ << token::END_STATEMENT << nl;
    os.writeKeyword("errorIntegral")
        << errorIntegral_ << token::END_STATEMENT << nl;

    writeEntry("value", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
   makePatchTypeField
   (
       fvPatchVectorField,
       pressurePIDControlInletVelocityFvPatchVectorField
   );
}


// ************************************************************************* //
