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

// OpenFOAM includes
#include "surface.H"
#include "runTimePostProcessing.H"

// VTK includes
#include "vtkActor.h"
#include "vtkFeatureEdges.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

namespace Foam
{
    template<>
    const char* NamedEnum<surface::representationType, 5>::names[] =
    {
        "none",
        "wireframe",
        "surface",
        "surfaceWithEdges",
        "glyph"
    };

    defineTypeNameAndDebug(surface, 0);
    defineRunTimeSelectionTable(surface, dictionary);
}

const Foam::NamedEnum<Foam::surface::representationType, 5>
    Foam::surface::representationTypeNames;


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

void Foam::surface::setRepresentation(vtkActor* actor) const
{
    geometryBase::initialiseActor(actor);

    switch (representation_)
    {
        case rtNone:
        {
            actor->VisibilityOff();
            break;
        }
        case rtWireframe:
        {
            // note: colour is set using general SetColour, not setEdgeColor
            actor->GetProperty()->SetRepresentationToWireframe();
            break;
        }
        case rtGlyph:
        case rtSurface:
        {
            actor->GetProperty()->SetRepresentationToSurface();
            break;
        }
        case rtSurfaceWithEdges:
        {
            actor->GetProperty()->SetRepresentationToSurface();
            actor->GetProperty()->EdgeVisibilityOn();
            break;
        }
    }
}


void Foam::surface::addFeatureEdges
(
    vtkRenderer* renderer,
    vtkPolyData* data
) const
{
    if (!featureEdges_)
    {
        return;
    }

    vtkSmartPointer<vtkFeatureEdges> featureEdges =
        vtkSmartPointer<vtkFeatureEdges>::New();
    featureEdges->SetInputData(data);
    featureEdges->BoundaryEdgesOn();
    featureEdges->FeatureEdgesOn();
    featureEdges->ManifoldEdgesOff();
    featureEdges->NonManifoldEdgesOff();
//    featureEdges->SetFeatureAngle(60);
    featureEdges->ColoringOff();
    featureEdges->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(featureEdges->GetOutputPort());
    mapper->ScalarVisibilityOff();

    edgeActor_->GetProperty()->SetSpecular(0);
    edgeActor_->GetProperty()->SetSpecularPower(20);
    edgeActor_->GetProperty()->SetRepresentationToWireframe();
    edgeActor_->SetMapper(mapper);

    renderer->AddActor(edgeActor_);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::surface::surface
(
    const runTimePostProcessing& parent,
    const dictionary& dict,
    const HashPtrTable<DataEntry<vector>, word>& colours
)
:
    geometryBase(parent, dict, colours),
    representation_
    (
        representationTypeNames.read(dict.lookup("representation"))
    ),
    featureEdges_(false),
    surfaceColour_(NULL),
    edgeColour_(NULL),
    surfaceActor_(),
    edgeActor_(),
    maxGlyphLength_(0.0)
{
    surfaceActor_ = vtkSmartPointer<vtkActor>::New();
    edgeActor_ = vtkSmartPointer<vtkActor>::New();

    if (dict.found("surfaceColour"))
    {
        surfaceColour_.reset
        (
            DataEntry<vector>::New("surfaceColour", dict).ptr()
        );
    }
    else
    {
        surfaceColour_.reset(colours["surface"]->clone().ptr());
    }

    if (dict.found("edgeColour"))
    {
        edgeColour_.reset(DataEntry<vector>::New("edgeColour", dict).ptr());
    }
    else
    {
        edgeColour_.reset(colours["edge"]->clone().ptr());
    }

    if (representation_ == rtGlyph)
    {
        dict.lookup("maxGlyphLength") >> maxGlyphLength_;
    }
    else
    {
        dict.lookup("featureEdges") >> featureEdges_;
    }
}


// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

Foam::autoPtr<Foam::surface> Foam::surface::New
(
    const runTimePostProcessing& parent,
    const dictionary& dict,
    const HashPtrTable<DataEntry<vector>, word>& colours,
    const word& surfaceType
)
{
    if (debug)
    {
        Info<< "Selecting surface " << surfaceType << endl;
    }

    dictionaryConstructorTable::iterator cstrIter =
        dictionaryConstructorTablePtr_->find(surfaceType);

    if (cstrIter == dictionaryConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "Foam::autoPtr<Foam::surface> Foam::surface::New"
            "("
                "const runTimePostProcessing&, "
                "const dictionary&, "
                "const HashPtrTable<DataEntry<vector>, word>&, "
                "const word&"
            ")"
        )   << "Unknown surface type "
            << surfaceType << nl << nl
            << "Valid surface types are:" << endl
            << dictionaryConstructorTablePtr_->sortedToc()
            << exit(FatalError);
    }

    return autoPtr<surface>(cstrIter()(parent, dict, colours));
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::surface::~surface()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::surface::updateActors(const scalar position)
{
    if (!featureEdges_)
    {
        return;
    }

    edgeActor_->GetProperty()->SetLineWidth(2);
    edgeActor_->GetProperty()->SetOpacity(opacity(position));

    const vector colour = edgeColour_->value(position);
    edgeActor_->GetProperty()->SetColor
    (
        colour[0],
        colour[1],
        colour[2]
    );
    edgeActor_->GetProperty()->SetEdgeColor
    (
        colour[0],
        colour[1],
        colour[2]
    );
}


// ************************************************************************* //
