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

#include "noiseFFT.H"
#include "IFstream.H"
#include "DynamicList.H"
#include "fft.H"
#include "SubField.H"
#include "mathematicalConstants.H"

using namespace Foam::constant;

// * * * * * * * * * * * * * * Static Member Data  * * * * * * * * * * * * * //

Foam::scalar Foam::noiseFFT::p0 = 2e-5;


Foam::tmp<Foam::scalarField> Foam::noiseFFT::frequencies
(
    const label N,
    const scalar deltaT
)
{
    tmp<scalarField> tf(new scalarField(N/2, 0));
    scalarField& f = tf();

    scalar deltaf = 1.0/(N*deltaT);
    forAll(f, i)
    {
        f[i] = i*deltaf;
    }

    return tf;
}


Foam::tmp<Foam::scalarField> Foam::noiseFFT::oneThirdOctaveFrequencies
(
    const scalarField& f,
    const scalar deltaT,
    const scalar f1,
    const scalar fU
)
{
    tmp<scalarField> tf13(new scalarField(f.size(), 0));
    scalarField& f13 = tf13();

    scalar fratio = cbrt(2.0);
    scalar deltaf = 1.0/(2*f.size()*deltaT);

    scalar fl = f1/sqrt(fratio);
    scalar fu = fratio*fl;

    label istart = floor(fl/deltaf);
    label j = 0;

    for (label i = istart; i < f.size(); i++)
    {
        scalar fmi = sqrt(fu*fl);

        if (fmi > fU + 1) break;

        if (f[i] >= fu)
        {
            f13[j] = fmi;
            fl = fu;
            fu *= fratio;
            j++;
        }
    }

    f13.setSize(j);

    return tf13;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::noiseFFT::noiseFFT
(
    const scalar deltat,
    const scalarField& pressure
)
:
    scalarField(pressure),
    deltat_(deltat)
{}


Foam::noiseFFT::noiseFFT(const fileName& pFileName, const label skip)
:
    scalarField(),
    deltat_(0.0)
{
    // Construct pressure data file
    IFstream pFile(pFileName);

    // Check pFile stream is OK
    if (!pFile.good())
    {
        FatalErrorIn("noiseFFT::noiseFFT(const fileName&, const label)")
            << "Cannot read file " << pFileName
            << exit(FatalError);
    }

    if (skip)
    {
        scalar dummyt, dummyp;

        for (label i = 0; i < skip; i++)
        {
            pFile >> dummyt;

            if (!pFile.good() || pFile.eof())
            {
                FatalErrorIn("noiseFFT::noiseFFT(const fileName&, const label)")
                    << "Number of points in file " << pFileName
                    << " is less than the number to be skipped = " << skip
                    << exit(FatalError);
            }

            pFile >> dummyp;
        }
    }

    scalar t = 0, T = 0;
    DynamicList<scalar> pData(100000);
    label i = 0;

    while (!(pFile >> t).eof())
    {
        T = t;
        pFile >> pData(i++);
    }

    deltat_ = T/pData.size();

    this->transfer(pData);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::graph Foam::noiseFFT::pt() const
{
    scalarField t(size());
    forAll(t, i)
    {
        t[i] = i*deltat_;
    }

    return graph
    (
        "p(t)",
        "t [s]",
        "p(t) [Pa]",
        t,
        *this
    );
}


Foam::tmp<Foam::scalarField> Foam::noiseFFT::window
(
    const label N,
    const label ni,
    const label nOverlap
) const
{
    label windowOffset = ni*(N - nOverlap);

    if ((windowOffset + N) > size())
    {
        FatalErrorIn("noiseFFT::window(const label, const label) const")
            << "Requested window is outside upper bounds of data" << nl
            << "number of data = " << size() << nl
            << "start index " << windowOffset + N << nl
            << "size of window = " << N << nl
            << "overlap = " << nOverlap << nl
            << "window index = " << ni
            << exit(FatalError);
    }

    tmp<scalarField> tpw(new scalarField(N));
    scalarField& pw = tpw();

    forAll(pw, i)
    {
        pw[i] = operator[](i + windowOffset);
    }

    return tpw;
}


Foam::tmp<Foam::scalarField> Foam::noiseFFT::Hanning(const label N) const
{
    scalarField t(N);
    forAll(t, i)
    {
        t[i] = i;
    }

    return 0.5*(1 - cos(mathematical::twoPi*t/scalar(N - 1)));
}


Foam::tmp<Foam::scalarField> Foam::noiseFFT::symmetricHanning
(
    const label N
) const
{
    label M = 0;
    if (N % 2 == 0)
    {
        M = N/2;
    }
    else
    {
        M = (N + 1)/2;
    }

    scalar alpha = 0.5; // Hanning = 0.5; Hamming = 0.54

    tmp<scalarField> tt(new scalarField(N));
    scalarField& t = tt();

    for (label n = 0; n < M; n++)
    {
        scalar h = alpha - (1 - alpha)*cos(mathematical::twoPi*n/scalar(N - 1));
        t[n] = h;
        t[N - 1 - n] = h;
    }

    return tt;
}


Foam::tmp<Foam::scalarField> Foam::noiseFFT::Pf
(
    const tmp<scalarField>& tpn
) const
{
    tmp<scalarField> tPn2
    (
        mag
        (
            fft::reverseTransform
            (
                ReComplexField(tpn),
                labelList(1, tpn().size())
            )
        )
    );

    tpn.clear();

    tmp<scalarField> tPn
    (
        new scalarField
        (
            scalarField::subField(tPn2(), tPn2().size()/2)
        )
    );
    scalarField& Pn = tPn();

    Pn *= 2.0/sqrt(scalar(tPn2().size()));
    Pn[0] /= 2.0;

    return tPn;
}


Foam::graph Foam::noiseFFT::meanPf
(
    const label N,
    const label nw,
    const label nOverlap
) const
{
    if (N > size())
    {
        FatalErrorIn("noiseFFT::meanPf(const label, const label) const")
            << "Requested window is outside set of data" << nl
            << "number of data = " << size() << nl
            << "size of window = " << N << nl
            << "window  = " << nw
            << exit(FatalError);
    }

    scalarField MeanPf(N/2, 0.0);

    scalarField Hwf(symmetricHanning(N));

    for (label wi = 0; wi < nw; ++wi)
    {
        MeanPf += Pf(Hwf*window(N, wi, nOverlap));
    }

    MeanPf /= nw;

    scalar deltaf = 1.0/(N*deltat_);
    scalarField f(MeanPf.size());
    forAll(f, i)
    {
        f[i] = i*deltaf;
    }

    return graph
    (
        "P(f)",
        "f [Hz]",
        "P(f) [Pa]",
        f,
        MeanPf
    );
}


Foam::graph Foam::noiseFFT::RMSmeanPf
(
    const label N,
    const label nw,
    const label nOverlap
) const
{
    if (N > size())
    {
        FatalErrorIn("noiseFFT::RMSmeanPf(const label, const label) const")
            << "Requested window is outside set of data" << nl
            << "number of data = " << size() << nl
            << "size of window = " << N << nl
            << "window  = " << nw
            << exit(FatalError);
    }

    scalarField RMSMeanPf(N/2, 0.0);

    scalarField Hwf(symmetricHanning(N));

    for (label wi = 0; wi < nw; ++wi)
    {
        RMSMeanPf += sqr(Pf(Hwf*window(N, wi, nOverlap)));
    }

    RMSMeanPf = sqrt(RMSMeanPf/nw);

    scalar deltaf = 1.0/(N*deltat_);
    scalarField f(RMSMeanPf.size());
    forAll(f, i)
    {
        f[i] = i*deltaf;
    }

    return graph
    (
        "P(f)",
        "f [Hz]",
        "P(f) [Pa]",
        f,
        RMSMeanPf
    );
}


Foam::graph Foam::noiseFFT::PSDf
(
    const label N,
    const label nw,
    const label nOverlap
) const
{
    if (N > size())
    {
        FatalErrorIn("noiseFFT::PSDf(const label, const label) const")
            << "Requested window is outside set of data" << endl
            << "number of data = " << size() << endl
            << "size of window = " << N << endl
            << "window  = " << nw
            << exit(FatalError);
    }

    scalarField psd(N/2, 0.0);

    scalarField Hwf(symmetricHanning(N));

    for (label wi = 0; wi < nw; ++wi)
    {
        psd += 0.5*sqr(Pf(Hwf*window(N, wi, nOverlap)));
    }

    scalar deltaf = 1.0/(N*deltat_);

    psd /= nw*deltaf;

    scalarField f(psd.size());

    forAll(f, i)
    {
        f[i] = i*deltaf;
    }

    return graph
    (
        "PSD(f)",
        "f [Hz]",
        "PSD(f) [PaPa_Hz]",
        f,
        psd
    );
}


Foam::graph Foam::noiseFFT::PSD(const graph& gPSD) const
{
    return graph
    (
        "PSD(dB)",
        "f [Hz]",
        "PSD_dB(f) [dB]",
        gPSD.x(),
        10*log10(gPSD.y()/sqr(p0))
    );
}


Foam::graph Foam::noiseFFT::Lf(const graph& gPf) const
{
    return graph
    (
        "L(f)",
        "f [Hz]",
        "L(f) [dB]",
        gPf.x(),
        20*log10(gPf.y()/p0)
    );
}


Foam::graph Foam::noiseFFT::Ldelta
(
    const graph& gLf,
    const scalar f1,
    const scalar fU
) const
{
    const scalarField& f = gLf.x();
    const scalarField& Lf = gLf.y();

    scalarField ldelta(Lf.size(), 0.0);
    scalarField fm(ldelta.size());

    scalar fratio = cbrt(2.0);
    scalar deltaf = 1.0/(2*Lf.size()*deltat_);

    scalar fl = f1/sqrt(fratio);
    scalar fu = fratio*fl;

    label istart = floor(fl/deltaf);
    label j = 0;

    for (label i = istart; i < Lf.size(); i++)
    {
        scalar fmi = sqrt(fu*fl);

        if (fmi > fU + 1) break;

        if (f[i] >= fu)
        {
            fm[j] = fmi;
            ldelta[j] = 10*log10(ldelta[j]);

            j++;

            fl = fu;
            fu *= fratio;
        }

        ldelta[j] += pow(10, Lf[i]/10.0);
    }

    fm.setSize(j);
    ldelta.setSize(j);

    return graph
    (
        "Ldelta",
        "fm [Hz]",
        "Ldelta [dB]",
        fm,
        ldelta
    );
}


Foam::graph Foam::noiseFFT::Pdelta
(
    const graph& gPf,
    const scalar f1,
    const scalar fU
) const
{
    const scalarField& f = gPf.x();
    const scalarField& Pf = gPf.y();

    scalarField pdelta(Pf.size(), 0.0);
    scalarField fm(pdelta.size());

    scalar fratio = cbrt(2.0);
    scalar deltaf = 1.0/(2*Pf.size()*deltat_);

    scalar fl = f1/sqrt(fratio);
    scalar fu = fratio*fl;

    label istart = floor(fl/deltaf);
    label j = 0;

    for (label i = istart; i < Pf.size(); i++)
    {
        scalar fmi = sqrt(fu*fl);

        if (fmi > fU + 1) break;

        if (f[i] >= fu)
        {
            fm[j] = fmi;
            pdelta[j] = sqrt((2.0/3.0)*pdelta[j]);

            j++;

            fl = fu;
            fu *= fratio;
        }

        pdelta[j] += sqr(Pf[i]);
    }

    fm.setSize(j);
    pdelta.setSize(j);

    return graph
    (
        "Pdelta",
        "fm [Hz]",
        "Pdelta [dB]",
        fm,
        pdelta
    );
}


Foam::scalar Foam::noiseFFT::Lsum(const graph& gLf) const
{
    const scalarField& Lf = gLf.y();

    scalar lsum = 0.0;

    forAll(Lf, i)
    {
        lsum += pow(10, Lf[i]/10.0);
    }

    lsum = 10*log10(lsum);

    return lsum;
}


Foam::scalar Foam::noiseFFT::dbToPa(const scalar db) const
{
    return p0*pow(10.0, db/20.0);
}


Foam::tmp<Foam::scalarField> Foam::noiseFFT::dbToPa
(
    const tmp<scalarField>& db
) const
{
    return p0*pow(10.0, db/20.0);
}


// ************************************************************************* //
