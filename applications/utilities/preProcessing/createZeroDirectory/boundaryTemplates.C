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

#include "boundaryTemplates.H"
#include "Time.H"
#include "IFstream.H"
#include "OStringStream.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::boundaryTemplates::boundaryTemplates
(
    const fileName& baseDir,
    const Time& runTime,
    const word& solverType
)
:
    templates_(dictionary::null),
    options_(dictionary::null)
{
    Info<< "    Reading boundary templates" << endl;

    fileName BCDir(baseDir/"boundaryConditions");

    IOdictionary regionBCs
    (
        IOobject
        (
            fileName(BCDir/"boundaries"),
            runTime,
            IOobject::MUST_READ
        )
    );

    forAllConstIter(dictionary, regionBCs, iter)
    {
        const word& regionType = iter().keyword();
        wordList patchTypes(regionBCs.lookup(regionType));

        dictionary regionTemplate = dictionary::null;
        dictionary regionOptions = dictionary::null;

        // read general boundary types
        forAll(patchTypes, i)
        {
            IOdictionary dict
            (
                IOobject
                (
                    fileName(BCDir/regionType/patchTypes[i]),
                    runTime,
                    IOobject::MUST_READ
                )
            );

            regionTemplate.add(patchTypes[i], dictionary(dict));
        }

        // add solver type boundary types
        forAll(patchTypes, i)
        {
            IOobject io
            (
                fileName(BCDir/regionType/solverType/patchTypes[i]),
                runTime,
                IOobject::MUST_READ
            );

            if (io.typeHeaderOk<IOdictionary>(true))
            {
                IOdictionary dict(io);
                regionTemplate.subDict(patchTypes[i]).merge(dict);
            }
        }

        // read general boundary options
        forAll(patchTypes, i)
        {
            fileName optFile(BCDir/regionType/patchTypes[i] + "Options");

            IFstream is(optFile);

            if (is.good())
            {
                IOdictionary dict
                (
                    IOobject
                    (
                        optFile,
                        runTime,
                        IOobject::MUST_READ
                    )
                );

                regionOptions.add(patchTypes[i], dictionary(dict));
            }
        }

        // add solver type boundary options
        forAll(patchTypes, i)
        {
            // options are optional - however, if file exists, assume that it
            // is to be read
            fileName optFile
            (
                BCDir/regionType/solverType/patchTypes[i] + "Options"
            );

            IFstream is(optFile);

            if (is.good())
            {
                IOdictionary dict
                (
                    IOobject
                    (
                        optFile,
                        runTime,
                        IOobject::MUST_READ
                    )
                );

                regionOptions.subDict(patchTypes[i]).merge(dict);
            }
        }

        templates_.add(regionType, regionTemplate);
        options_.add(regionType, regionOptions);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::dictionary& Foam::boundaryTemplates::templates() const
{
    return templates_;
}


Foam::dictionary Foam::boundaryTemplates::generatePatchDict
(
    const word& regionPrefix,
    const word& fieldName,
    const word& condition,
    const word& category,
    const word& patchType,
    const dictionary& conditionOptions
) const
{
    const dictionary& regionTemplates = templates_.subDict(regionPrefix);

    // look for inlet, outlet, wall etc
    if (regionTemplates.found(category))
    {
        const dictionary& categoryDict(regionTemplates.subDict(category));

        // look for subSonic, slip etc
        if (categoryDict.found(patchType))
        {
            dictionary patchDict = categoryDict.subDict(patchType);

            // add any options
            if (patchDict.found("OPTIONS"))
            {
                const dictionary& regionOptions =
                    options_.subDict(regionPrefix);

                if (!regionOptions.found(category))
                {
                    FatalError<< "No options available for category "
                        << category << exit(FatalError);
                }

                const dictionary& dict = regionOptions.subDict(category);

                const wordList requiredOptions(patchDict.lookup("OPTIONS"));

                forAll(requiredOptions, i)
                {
                    const word& option = requiredOptions[i];

                    word selected;
                    if (!conditionOptions.readIfPresent(option, selected))
                    {
                        FatalErrorIn
                        (
                            "Foam::dictionary "
                            "Foam::boundaryTemplates::generatePatchDict"
                            "("
                                "const word&, "
                                "const word&, "
                                "const word&, "
                                "const word&, "
                                "const word&, "
                                "const dictionary&"
                            ") const"
                        )
                            << "Condition " << condition << ": "
                            << "No option '" << option
                            << "' available for category '" << category
                            << "' and patch type '" << patchType
                            << "'.  Valid options are: "
                            << conditionOptions.toc()
                            << exit(FatalError);
                    }

                    if (!dict.found(option))
                    {
                        FatalErrorIn
                        (
                            "Foam::dictionary "
                            "Foam::boundaryTemplates::generatePatchDict"
                            "("
                                "const word&, "
                                "const word&, "
                                "const word&, "
                                "const word&, "
                                "const word&, "
                                "const dictionary&"
                            ") const"
                        )
                            << "Condition " << condition << ": "
                            << "No option '" << option
                            << "' available for category '" << category
                            << "' and patch type '" << patchType
                            << "'.  Valid options are " << dict.toc()
                            << exit(FatalError);
                    }

                    const dictionary& optionDict = dict.subDict(option);

                    if (!optionDict.found(selected))
                    {
                        FatalErrorIn
                        (
                            "Foam::dictionary "
                            "Foam::boundaryTemplates::generatePatchDict"
                            "("
                                "const word&, "
                                "const word&, "
                                "const word&, "
                                "const word&, "
                                "const word&, "
                                "const dictionary&"
                            ") const"
                        )
                            << "Condition " << condition << ": "
                            << "No option '" << selected
                            << "' available for category '" << category
                            << "' and patch type '" << patchType
                            << "'.  Valid options are " << optionDict.toc()
                            << exit(FatalError);
                    }

                    const dictionary& dict = optionDict.subDict(selected);

                    patchDict.merge(dict);
                }
            }

            // look for field name
            if (patchDict.found(fieldName))
            {
                dictionary dict(dictionary::null);
                const dictionary& fieldDict(patchDict.subDict(fieldName));

                forAllConstIter(IDLList<entry>, fieldDict, iter)
                {
                    OStringStream oss;
                    oss << iter();
                    string s(oss.str());
                    s.replace(iter().keyword(), "");
                    s.replace
                    (
                        "VALUE",
                        "boundaryConditions." + condition + ".values"
                    );
                    dict.add(iter().keyword(), s.c_str());
                }

                return dict;
            }
            else
            {
                FatalErrorIn
                (
                    "Foam::dictionary "
                    "Foam::boundaryTemplates::generatePatchDict"
                    "("
                        "const word&, "
                        "const word&, "
                        "const word&, "
                        "const word&, "
                        "const word&, "
                        "const dictionary&"
                    ") const"
                )
                    << "Condition " << condition << ": "
                    << "No '" << patchType
                    << "' condition found for field '" << fieldName
                    << "' in category type '" << category << "'"
                    << exit(FatalError);
            }
        }
        else
        {
            FatalErrorIn
            (
                "Foam::dictionary "
                "Foam::boundaryTemplates::generatePatchDict"
                "("
                    "const word&, "
                    "const word&, "
                    "const word&, "
                    "const word&, "
                    "const word&, "
                    "const dictionary&"
                ") const"
            )
                << "Condition " << condition << ": "
                << "No '" << patchType << "' boundary types defined in "
                << categoryDict.dictName() << " templates.  "
                << "Available types are: " << categoryDict.toc()
                << exit(FatalError);
        }
    }
    else
    {
        FatalErrorIn
        (
            "Foam::dictionary "
            "Foam::boundaryTemplates::generatePatchDict"
            "("
                "const word&, "
                "const word&, "
                "const word&, "
                "const word&, "
                "const word&, "
                "const dictionary&"
            ") const"
        )
            << "Condition " << condition << ": "
            << "Invalid boundary condition type '" << patchType
            << "'.  Valid types are:" << regionTemplates.toc()
            << exit(FatalError);
    }

    return dictionary::null;
}


void Foam::boundaryTemplates::checkPatch
(
    const word& regionPrefix,
    const word& condition,
    const word& category,
    const word& patchType
) const
{
    const dictionary& regionTemplates = templates_.subDict(regionPrefix);

    if (!regionTemplates.found(category))
    {
        FatalErrorIn
        (
            "void Foam::boundaryTemplates::checkPatch"
            "("
                "const word&, "
                "const word&"
            ") const"
        )
            << "Condition " << condition << ": "
            << "Unknown category '" << category
            << "'.  Valid categories are: " << regionTemplates.toc()
            << exit(FatalError);
    }

    const dictionary& categoryDict = regionTemplates.subDict(category);

    if (!categoryDict.found(patchType))
    {
        FatalErrorIn
        (
            "void Foam::boundaryTemplates::checkPatch"
            "("
                "const word&, "
                "const word&"
            ") const"
        )
            << "Condition " << condition << ": "
            << "Unknown type '" << patchType << "' in category '"
            << category << "'.  Valid types are: " << categoryDict.toc()
            << exit(FatalError);
    }
}


bool Foam::boundaryTemplates::optionsRequired
(
    const word& regionPrefix,
    const word& category,
    const word& patchType
) const
{
    const dictionary& regionTemplates = templates_.subDict(regionPrefix);

    if (regionTemplates.found(category))
    {
        const dictionary& categoryDict(regionTemplates.subDict(category));

        if (categoryDict.found(patchType))
        {
            const dictionary& patchDict = categoryDict.subDict(patchType);

            if (patchDict.found("OPTIONS"))
            {
                return true;
            }
        }
        else
        {
            FatalErrorIn
            (
                "bool Foam::boundaryTemplates::optionsRequired"
                "("
                    "const word&, "
                    "const word&"
                ") const"
            )
                << "No type '" << patchType << "' found in category '"
                << category << "'.  Valid types are "
                << categoryDict.toc()
                << exit(FatalError);
        }
    }
    else
    {
        FatalErrorIn
        (
            "bool Foam::boundaryTemplates::optionsRequired"
            "("
                "const word&, "
                "const word&"
            ") const"
        )
            << "No category '" << category << "' found in templates.  "
            << "Valid categories are " << templates_.toc()
            << exit(FatalError);
    }

    return false;
}


// ************************************************************************* //
