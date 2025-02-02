/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include <fstream>
#include "itkBilateralImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"
#include "itkTestingMacros.h"

int
itkBilateralImageFilterTest2(int argc, char * argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << itkNameOfTestExecutableMacro(argv) << " InputImage OutputImage\n";
    return -1;
  }

  using PixelType = unsigned char;
  constexpr unsigned int dimension = 2;
  using myImage = itk::Image<PixelType, dimension>;
  itk::ImageFileReader<myImage>::Pointer input = itk::ImageFileReader<myImage>::New();
  input->SetFileName(argv[1]);

  // Create a filter
  using FilterType = itk::BilateralImageFilter<myImage, myImage>;

  auto                     filter = FilterType::New();
  itk::SimpleFilterWatcher watcher(filter, "filter");

  filter->SetInput(input->GetOutput());

  // these settings reduce the amount of noise by a factor of 10
  // when the original signal to noise level is 5
  filter->SetDomainSigma(4.0);
  filter->SetRangeSigma(50.0);
  filter->SetDomainMu(2.5);

  // Test itkSetVectorMacro
  double domainSigma[dimension];
  for (double & i : domainSigma)
  {
    i = 4.0;
  }
  filter->SetDomainSigma(domainSigma);

  // Test itkGetVectorMacro
  std::cout << "filter->GetDomainSigma(): " << filter->GetDomainSigma() << std::endl;

  // Test itkSetMacro
  unsigned int  filterDimensionality = dimension;
  unsigned long numberOfRangeGaussianSamples = 100;
  filter->SetFilterDimensionality(filterDimensionality);
  filter->SetNumberOfRangeGaussianSamples(numberOfRangeGaussianSamples);

  // Test itkGetMacro
  double rangeSigma2 = filter->GetRangeSigma();
  std::cout << "filter->GetRangeSigma(): " << rangeSigma2 << std::endl;
  unsigned int filterDimensionality2 = filter->GetFilterDimensionality();
  std::cout << "filter->GetFilterDimensionality(): " << filterDimensionality2 << std::endl;
  unsigned long numberOfRangeGaussianSamples2 = filter->GetNumberOfRangeGaussianSamples();
  std::cout << "filter->GetNumberOfRangeGaussianSamples(): " << numberOfRangeGaussianSamples2 << std::endl;
  double domainMu = filter->GetDomainMu();
  std::cout << "filter->GetDomainMu(): " << domainMu << std::endl;

  try
  {
    input->Update();
    filter->Update();
  }
  catch (const itk::ExceptionObject & e)
  {
    std::cerr << "Exception detected: " << e.GetDescription();
    return -1;
  }
  catch (...)
  {
    std::cerr << "Some other exception occurred" << std::endl;
    return -2;
  }

  // Generate test image
  itk::ImageFileWriter<myImage>::Pointer writer;
  writer = itk::ImageFileWriter<myImage>::New();
  writer->SetInput(filter->GetOutput());
  writer->SetFileName(argv[2]);
  writer->Update();

  return EXIT_SUCCESS;
}
