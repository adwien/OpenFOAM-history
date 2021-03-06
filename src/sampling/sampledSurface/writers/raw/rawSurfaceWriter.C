/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2014 OpenFOAM Foundation
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

#include "rawSurfaceWriter.H"
#include "makeSurfaceWriterMethods.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    makeSurfaceWriterType(rawSurfaceWriter);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

inline void Foam::rawSurfaceWriter::writeLocation
(
    Ostream& os,
    const pointField& points,
    const label pointI
)
{
    const point& pt = points[pointI];
    os  << pt.x() << ' ' << pt.y() << ' ' << pt.z() << ' ';
}


inline void Foam::rawSurfaceWriter::writeLocation
(
    Ostream& os,
    const pointField& points,
    const faceList& faces,
    const label faceI
)
{
    const point& ct = faces[faceI].centre(points);
    os  << ct.x() << ' ' << ct.y() << ' ' << ct.z() << ' ';
}


namespace Foam
{
    template<>
    void Foam::rawSurfaceWriter::writeHeader
    (
        Ostream& os,
        const word& fieldName,
        const Field<scalar>& values
    )
    {
        os  << values.size() << nl
            << "#  x  y  z  " << fieldName << nl;
    }


    template<>
    void Foam::rawSurfaceWriter::writeHeader
    (
        Ostream& os,
        const word& fieldName,
        const Field<vector>& values
    )
    {
        os  << values.size() << nl
            << "#  x  y  z  "
            << fieldName << "_x  "
            << fieldName << "_y  "
            << fieldName << "_z  "
            << endl;
    }


    template<>
    void Foam::rawSurfaceWriter::writeHeader
    (
        Ostream& os,
        const word& fieldName,
        const Field<sphericalTensor>& values
    )
    {
        os  << values.size() << nl
            << "#  ii  "
            << fieldName << "_ii" << nl;
    }


    template<>
    void Foam::rawSurfaceWriter::writeHeader
    (
        Ostream& os,
        const word& fieldName,
        const Field<symmTensor>& values
    )
    {
        os  << values.size() << nl
            << "#  xx  xy  xz  yy  yz ";
        for (int i=0; i<6; ++i)
        {
            os  << fieldName << "_" << i << "  ";
        }
        os  << endl;
    }


    template<>
    void Foam::rawSurfaceWriter::writeHeader
    (
        Ostream& os,
        const word& fieldName,
        const Field<tensor>& values
    )
    {
        os  << values.size() << nl
            << "#  xx  xy  xz  yx  yy  yz  zx  zy  zz";
        for (int i=0; i<9; ++i)
        {
            os  << fieldName << "_" << i << "  ";
        }
        os  << nl;
    }


    template<>
    inline void Foam::rawSurfaceWriter::writeData
    (
        Ostream& os,
        const scalar& v
    )
    {
        os  << v << nl;
    }


    template<>
    inline void Foam::rawSurfaceWriter::writeData
    (
        Ostream& os,
        const vector& v
    )
    {
        os  << v[0] << ' ' << v[1] << ' ' << v[2] << nl;
    }


    template<>
    inline void Foam::rawSurfaceWriter::writeData
    (
        Ostream& os,
        const sphericalTensor& v
    )
    {
        os  << v[0] << nl;
    }


    template<>
    inline void Foam::rawSurfaceWriter::writeData
    (
        Ostream& os,
        const symmTensor& v
    )
    {
        os  << v[0] << ' ' << v[1] << ' ' << v[2] << ' '
            << v[3] << ' ' << v[4] << ' ' << v[5] << nl;
    }


    template<>
    inline void Foam::rawSurfaceWriter::writeData
    (
        Ostream& os,
        const tensor& v
    )
    {
        os  << v[0] << ' ' << v[1] << ' ' << v[2] << ' '
            << v[3] << ' ' << v[4] << ' ' << v[5] << ' '
            << v[6] << ' ' << v[7] << ' ' << v[8] << nl;
    }

}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::rawSurfaceWriter::rawSurfaceWriter()
:
    surfaceWriter()
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::rawSurfaceWriter::~rawSurfaceWriter()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::fileName Foam::rawSurfaceWriter::write
(
    const fileName& outputDir,
    const fileName& surfaceName,
    const pointField& points,
    const faceList& faces,
    const bool verbose
) const
{
    if (!isDir(outputDir))
    {
        mkDir(outputDir);
    }

    OFstream os(outputDir/surfaceName + ".raw");

    if (verbose)
    {
        Info<< "Writing geometry to " << os.name() << endl;
    }


    // header
    os  << "# geometry NO_DATA " << faces.size() << nl
        << "#  x  y  z" << nl;

    // Write faces centres
    forAll(faces, elemI)
    {
        writeLocation(os, points, faces, elemI);
        os  << nl;
    }

    os  << nl;

    return os.name();
}


// create write methods
defineSurfaceWriterWriteFields(Foam::rawSurfaceWriter);


// ************************************************************************* //
