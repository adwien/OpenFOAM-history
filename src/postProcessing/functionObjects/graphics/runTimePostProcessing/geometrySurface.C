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
#include "geometrySurface.H"
#include "stringOps.H"
#include "triSurface.H"
#include "runTimePostProcessing.H"
#include "addToRunTimeSelectionTable.H"

// VTK includes
#include "vtkActor.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkTriangle.h"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(geometrySurface, 0);
    addToRunTimeSelectionTable(surface, geometrySurface, dictionary);
}


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

void Foam::geometrySurface::addGeometryToScene
(
    const scalar position,
    vtkRenderer* renderer,
    const fileName& fName
) const
{
    if (representation_ == rtGlyph)
    {
        FatalErrorIn
        (
            "void Foam::geometrySurface::addGeometryToScene"
            "("
                "const label, "
                "vtkRenderer*, "
                "const fileName&"
            ") const"
        )
            << "Glyph representation not available for " << typeName
            << "object" << exit(FatalError);
    }

    triSurface surf(fName);

    const Field<point>& surfPoints = surf.points();
    const Field<vector>& surfFaceNormals = surf.faceNormals();

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> triangles =
        vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkDoubleArray> faceNormals =
        vtkSmartPointer<vtkDoubleArray>::New();

    faceNormals->SetNumberOfComponents(3);

    forAll(surfPoints, i)
    {
        const point& pt = surfPoints[i];
        points->InsertNextPoint(pt.x(), pt.y(), pt.z());
    }

    forAll(surf, i)
    {
        const Foam::face& f = surf[i];

        vtkSmartPointer<vtkTriangle> triangle =
            vtkSmartPointer<vtkTriangle>::New();
        triangle->GetPointIds()->SetId(0, f[0]);
        triangle->GetPointIds()->SetId(1, f[1]);
        triangle->GetPointIds()->SetId(2, f[2]);
        triangles->InsertNextCell(triangle);

        double n[3];
        n[0] = surfFaceNormals[i].x();
        n[1] = surfFaceNormals[i].y();
        n[2] = surfFaceNormals[i].z();

        faceNormals->InsertNextTuple(n);
    }

    surf.clearOut();

    vtkSmartPointer<vtkPolyData> polyData =
        vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->SetPolys(triangles);
    polyData->GetCellData()->SetNormals(faceNormals);

    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->ScalarVisibilityOff();
    mapper->SetInputData(polyData);

    addFeatureEdges(renderer, polyData);

    surfaceActor_->SetMapper(mapper);

    setRepresentation(surfaceActor_);

    renderer->AddActor(surfaceActor_);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::geometrySurface::geometrySurface
(
    const runTimePostProcessing& parent,
    const dictionary& dict,
    const HashPtrTable<DataEntry<vector>, word>& colours
)
:
    surface(parent, dict, colours),
    fileNames_(dict.lookup("files"))
{}


Foam::geometrySurface::geometrySurface
(
    const runTimePostProcessing& parent,
    const dictionary& dict,
    const HashPtrTable<DataEntry<vector>, word>& colours,
    const List<fileName>& fileNames
)
:
    surface(parent, dict, colours),
    fileNames_(fileNames)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::geometrySurface::~geometrySurface()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::geometrySurface::addGeometryToScene
(
    const scalar position,
    vtkRenderer* renderer
)
{
    if (!visible_)
    {
        return;
    }

    forAll(fileNames_, i)
    {
        fileName fName = fileNames_[i].expand();
        addGeometryToScene(position, renderer, fName);
    }
}


void Foam::geometrySurface::updateActors(const scalar position)
{
    if (!visible_)
    {
        return;
    }

    surface::updateActors(position);

    surfaceActor_->GetProperty()->SetOpacity(opacity(position));

    vector sc = surfaceColour_->value(position);
    surfaceActor_->GetProperty()->SetColor(sc[0], sc[1], sc[2]);

    vector ec = edgeColour_->value(position);
    surfaceActor_->GetProperty()->SetEdgeColor(ec[0], ec[1], ec[2]);
}


// ************************************************************************* //
