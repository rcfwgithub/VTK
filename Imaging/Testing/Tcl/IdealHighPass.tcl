package require vtktcl

# This script shows the result of an ideal highpass filter in  spatial domain


# Image pipeline

vtkPNGReader reader
reader SetFileName $VTK_DATA_ROOT/Data/fullhead15.png

vtkImageFFT fft
fft SetInput [reader GetOutput]

vtkImageIdealHighPass highPass
highPass SetInput [fft GetOutput]
highPass SetXCutOff 0.1
highPass SetYCutOff 0.1
highPass ReleaseDataFlagOff

vtkImageRFFT rfft
rfft SetInput [highPass GetOutput]

vtkImageExtractComponents real
real SetInput [rfft GetOutput]
real SetComponents 0


vtkImageViewer viewer
viewer SetInput [real GetOutput]
viewer SetColorWindow 500
viewer SetColorLevel 0


viewer Render







