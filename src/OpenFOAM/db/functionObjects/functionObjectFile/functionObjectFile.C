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

#include "functionObjectFile.H"
#include "Time.H"
#include "polyMesh.H"
#include "IFstream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

const Foam::word Foam::functionObjectFile::outputPrefix = "postProcessing";
Foam::label Foam::functionObjectFile::addChars = 7;

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

void Foam::functionObjectFile::initStream(Ostream& os) const
{
    os.setf(ios_base::scientific, ios_base::floatfield);
    os.precision(writePrecision_);
    os.width(charWidth());
}


Foam::fileName Foam::functionObjectFile::baseFileDir() const
{
    fileName baseDir = obr_.time().path();

    if (Pstream::parRun())
    {
        // Put in undecomposed case (Note: gives problems for
        // distributed data running)
        baseDir = baseDir/".."/outputPrefix;
    }
    else
    {
        baseDir = baseDir/outputPrefix;
    }

    // append mesh name if not default region
    if (isA<polyMesh>(obr_))
    {
        const polyMesh& mesh = refCast<const polyMesh>(obr_);
        if (mesh.name() != polyMesh::defaultRegion)
        {
            baseDir = baseDir/mesh.name();
        }
    }

    return baseDir;
}


Foam::fileName Foam::functionObjectFile::baseTimeDir() const
{
    return baseFileDir()/prefix_/obr_.time().timeName();
}


Foam::autoPtr<Foam::OFstream> Foam::functionObjectFile::createFile
(
    const word& name
) const
{
    autoPtr<OFstream> osPtr;

    if (Pstream::master() && writeToFile_)
    {
        const word startTimeName =
            obr_.time().timeName(obr_.time().startTime().value());

        fileName outputDir(baseFileDir()/prefix_/startTimeName);

        mkDir(outputDir);

        word fName(name);

        // check if file already exists
        IFstream is(outputDir/(fName + ".dat"));
        if (is.good())
        {
            fName = fName + "_" + obr_.time().timeName();
        }

        osPtr.set(new OFstream(outputDir/(fName + ".dat")));

        initStream(osPtr());
    }

    return osPtr;
}


void Foam::functionObjectFile::resetFile(const word& fileName)
{
    fileName_ = fileName;
    filePtr_ = createFile(fileName_);
}


Foam::Omanip<int> Foam::functionObjectFile::valueWidth(const label offset) const
{
    return setw(IOstream::defaultPrecision() + addChars + offset);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjectFile::functionObjectFile
(
    const objectRegistry& obr,
    const word& prefix
)
:
    obr_(obr),
    prefix_(prefix),
    fileName_("undefined"),
    filePtr_(),
    writePrecision_(IOstream::defaultPrecision()),
    writeToFile_(true)
{}


Foam::functionObjectFile::functionObjectFile
(
    const objectRegistry& obr,
    const word& prefix,
    const word& fileName,
    const dictionary& dict
)
:
    obr_(obr),
    prefix_(prefix),
    fileName_(fileName),
    filePtr_(),
    writePrecision_(IOstream::defaultPrecision()),
    writeToFile_(true)
{
    read(dict);

    if (writeToFile_)
    {
        filePtr_ = createFile(fileName_);
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::functionObjectFile::~functionObjectFile()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::functionObjectFile::read(const dictionary& dict)
{
    writePrecision_ =
        dict.lookupOrDefault("writePrecision", IOstream::defaultPrecision());

    // Only write on master process
    writeToFile_ = dict.lookupOrDefault("writeToFile", true);
    writeToFile_ = writeToFile_ && Pstream::master();
}


Foam::OFstream& Foam::functionObjectFile::file()
{
    if (!writeToFile_)
    {
        return Snull;
    }

    if (!filePtr_.valid())
    {
        FatalErrorIn("Foam::OFstream& Foam::functionObjectFile::file()")
            << "File pointer not allocated"
            << abort(FatalError);
    }

    return filePtr_();
}


bool Foam::functionObjectFile::writeToFile() const
{
    return writeToFile_;
}


Foam::label Foam::functionObjectFile::charWidth() const
{
    return IOstream::defaultPrecision() + addChars;
}


void Foam::functionObjectFile::writeCommented
(
    Ostream& os,
    const string& str
) const
{
    os  << setw(1) << "#" << setw(1) << ' '
        << setw(charWidth() - 2) << str.c_str();
}


void Foam::functionObjectFile::writeTabbed
(
    Ostream& os,
    const string& str
) const
{
    os  << tab << setw(charWidth()) << str.c_str();
}


void Foam::functionObjectFile::writeHeader
(
    Ostream& os,
    const string& str
) const
{
    os  << setw(1) << "#" << setw(1) << ' '
        << setf(ios_base::left) << setw(charWidth() - 2) << str.c_str() << nl;
}


// ************************************************************************* //
