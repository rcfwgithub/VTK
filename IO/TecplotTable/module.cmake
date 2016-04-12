vtk_module(vtkIOTecplotTable
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonDataModel
    vtkCommonSystem
    vtkCommonMisc
    vtkIOCore
    vtkIOLegacy
    vtkIOXML
  TEST_DEPENDS
    vtkTestingCore
    vtkTestingRendering
  PRIVATE_DEPENDS
    vtksys
  )
