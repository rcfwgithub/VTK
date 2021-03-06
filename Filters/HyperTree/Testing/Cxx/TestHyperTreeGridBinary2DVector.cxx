/*==================================================================

  Program:   Visualization Toolkit
  Module:    TestHyperTreeGridBinary2DVector.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

===================================================================*/
// .SECTION Thanks
// This test was written by Philippe Pebay, 2016
// This work was supported by Commissariat a l'Energie Atomique (CEA/DIF)

#include "vtkHyperTreeGrid.h"
#include "vtkHyperTreeGridCellCenters.h"
#include "vtkHyperTreeGridGeometry.h"
#include "vtkHyperTreeGridSource.h"

#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkGlyph2D.h"
#include "vtkGlyphSource2D.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

int TestHyperTreeGridBinary2DVector( int argc, char* argv[] )
{
  // Hyper tree grid
  vtkNew<vtkHyperTreeGridSource> htGrid;
  int maxLevel = 6;
  htGrid->SetMaximumLevel( maxLevel );
  htGrid->SetGridSize( 2, 3, 1 );
  htGrid->SetGridScale( 1.5, 1., 10. );  // this is to test that orientation fixes scale
  htGrid->SetDimension( 2 );
  htGrid->SetOrientation( 2 ); // in xy plane
  htGrid->SetBranchFactor( 2 );
  htGrid->SetDescriptor( "RRRRR.|.... .R.. RRRR R... R...|.R.. ...R ..RR .R.. R... .... ....|.... ...R ..R. .... .R.. R...|.... .... .R.. ....|...." );
  htGrid->GenerateVectorFieldOn();
  htGrid->Update();
  vtkHyperTreeGrid* H = vtkHyperTreeGrid::SafeDownCast( htGrid->GetOutput() );
  H->SetHasInterface( 1 );
  char normalsName[] = "Vector";
  H->SetInterfaceNormalsName( normalsName );
  char interceptsName[] = "Depth";
  H->SetInterfaceInterceptsName( interceptsName );

  // Cell centers
  vtkNew<vtkHyperTreeGridCellCenters> centers;
  centers->SetInputConnection( htGrid->GetOutputPort() );
  centers->VertexCellsOn();

  // 2D glyph source
  vtkNew<vtkGlyphSource2D> glyph;
  glyph->SetGlyphTypeToArrow();
  glyph->SetScale( .8 );
  glyph->FilledOff();

  // Glyphs
  vtkNew<vtkGlyph2D> glypher;
  glypher->SetInputConnection( centers->GetOutputPort() );
  glypher->SetSourceConnection( glyph->GetOutputPort() );
  glypher->SetScaleModeToScaleByVector();
  glypher->OrientOn();

  // Geometries
  vtkNew<vtkHyperTreeGridGeometry> geometry1;
  geometry1->SetInputConnection( htGrid->GetOutputPort() );
  geometry1->Update();
  vtkPolyData* pd = geometry1->GetPolyDataOutput();
  vtkNew<vtkHyperTreeGridGeometry> geometry2;
  geometry2->SetInputConnection( htGrid->GetOutputPort() );

  // Mappers
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
  vtkNew<vtkPolyDataMapper> mapper1;
  mapper1->SetInputConnection( geometry1->GetOutputPort() );
  mapper1->ScalarVisibilityOff();
  vtkNew<vtkPolyDataMapper> mapper2;
  mapper2->SetInputConnection( geometry2->GetOutputPort() );
  mapper2->SetScalarRange( pd->GetCellData()->GetScalars()->GetRange() );
  vtkNew<vtkPolyDataMapper> mapper3;
  mapper3->SetInputConnection( glypher->GetOutputPort() );
  mapper3->ScalarVisibilityOff();

  // Actors
  vtkNew<vtkActor> actor1;
  actor1->SetMapper( mapper1.GetPointer() );
  actor1->GetProperty()->SetRepresentationToWireframe();
  actor1->GetProperty()->SetColor( .7, .7, .7 );
  vtkNew<vtkActor> actor2;
  actor2->SetMapper( mapper2.GetPointer() );
  vtkNew<vtkActor> actor3;
  actor3->SetMapper( mapper3.GetPointer() );
  actor3->GetProperty()->SetColor( 0., 0., 0. );
  actor3->GetProperty()->SetLineWidth( 1 );

  // Camera
  double bd[6];
  pd->GetBounds( bd );
  vtkNew<vtkCamera> camera;
  camera->SetClippingRange( 1., 100. );
  camera->SetFocalPoint( pd->GetCenter() );
  camera->SetPosition( .5 * bd[1], .5 * bd[3], 6. );

  // Renderer
  vtkNew<vtkRenderer> renderer;
  renderer->SetActiveCamera( camera.GetPointer() );
  renderer->SetBackground( 1., 1., 1. );
  renderer->AddActor( actor1.GetPointer() );
  renderer->AddActor( actor2.GetPointer() );
  renderer->AddActor( actor3.GetPointer() );

  // Render window
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer( renderer.GetPointer() );
  renWin->SetSize( 400, 400 );
  renWin->SetMultiSamples( 0 );

  // Interactor
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow( renWin.GetPointer() );

  // Render and test
  renWin->Render();

  int retVal = vtkRegressionTestImageThreshold( renWin.GetPointer(), 70 );
  if ( retVal == vtkRegressionTester::DO_INTERACTOR )
  {
    iren->Start();
  }

  return !retVal;
}
