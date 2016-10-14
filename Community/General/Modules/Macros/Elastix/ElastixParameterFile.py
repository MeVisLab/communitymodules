#----------------------------------------------------------------------------------
#
# Copyright (c) 2011, Image Sciences Institute, UMC Utrecht.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of ISI nor the names of its contributors 
#       may be used to endorse or promote products derived from this software 
#       without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ISI BE LIABLE FOR ANY DIRECT, INDIRECT, 
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#----------------------------------------------------------------------------------
#! Macro module ElastixParameterFile
#/*!
# \file    ElastixParameterFile.py
# \author  Marijn van Stralen
# \date    2009-10-20
#
# Create a parameter file for elastix
#*/
#----------------------------------------------------------------------------------

from mevis import *
import os.path

def write():
  dim  = int(ctx.field("FixedImageDimension").value);
  nres = int(ctx.field("NumberOfResolutions").value);
  
  paramStr = ctx.field("template").value;
  paramStr = paramStr.replace('P_FixedInternalImagePixelType', str(ctx.field("FixedInternalImagePixelType").value));
  paramStr = paramStr.replace('P_MovingInternalImagePixelType', str(ctx.field("MovingInternalImagePixelType").value));
  paramStr = paramStr.replace('P_FixedImageDimension', str(ctx.field("FixedImageDimension").value));
  paramStr = paramStr.replace('P_MovingImageDimension', str(ctx.field("MovingImageDimension").value));
  paramStr = paramStr.replace('P_Registration', str(ctx.field("Registration").value));
  paramStr = paramStr.replace('P_FixedImagePyramid', str(ctx.field("FixedImagePyramid").value));
  paramStr = paramStr.replace('P_MovingImagePyramid', str(ctx.field("MovingImagePyramid").value));
  paramStr = paramStr.replace('P_Interpolator', str(ctx.field("Interpolator").value));
  paramStr = paramStr.replace('P_ResampleInterpolator', str(ctx.field("ResampleInterpolator").value));
  paramStr = paramStr.replace('P_Resampler', str(ctx.field("Resampler").value));
  paramStr = paramStr.replace('P_Optimizer', str(ctx.field("Optimizer").value));
  paramStr = paramStr.replace('P_Transform', str(ctx.field("Transform").value));
  paramStr = paramStr.replace('P_Metric', str(ctx.field("Metric").value));
  paramStr = paramStr.replace('P_AutomaticScalesEstimation', bool2str(ctx.field("AutomaticScalesEstimation").value));
  paramStr = paramStr.replace('P_AutomaticTransformInitialization', bool2str(ctx.field("AutomaticTransformInitialization").value));
  paramStr = paramStr.replace('P_CenterOfRotation', vec2str(ctx.field("CenterOfRotation").value, dim));
  paramStr = paramStr.replace('P_NumberOfResolutions', str(ctx.field("NumberOfResolutions").value));
  paramStr = paramStr.replace('P_ResultImagePixelType', str(ctx.field("ResultImagePixelType").value));
  paramStr = paramStr.replace('P_ErodeMask', strList(bool2str(ctx.field("ErodeMask").value), nres) );
  paramStr = paramStr.replace('P_HowToCombineTransforms', str(ctx.field("HowToCombineTransforms").value));
  paramStr = paramStr.replace('P_NumberOfSpatialSamples', vec2intstr(ctx.field("NumberOfSpatialSamples").value, nres));
  paramStr = paramStr.replace('P_NewSamplesEveryIteration', strList(bool2str(ctx.field("NewSamplesEveryIteration").value), nres));
  paramStr = paramStr.replace('P_ImageSampler', str(ctx.field("ImageSampler").value));
  paramStr = paramStr.replace('P_NumberOfHistogramBins', vec2intstr(ctx.field("NumberOfHistogramBins").value, nres));
  paramStr = paramStr.replace('P_BSplineInterpolationOrder', vec2intstr(ctx.field("BSplineInterpolationOrder").value, nres));
  paramStr = paramStr.replace('P_FinalBSplineInterpolationOrder', str(ctx.field("FinalBSplineInterpolationOrder").value));
  paramStr = paramStr.replace('P_DefaultPixelValue', str(ctx.field("DefaultPixelValue").value));
  paramStr = paramStr.replace('P_MaximumNumberOfIterations', vec2intstr(ctx.field("MaximumNumberOfIterations").value, nres));
  paramStr = paramStr.replace('P_AutomaticParameterEstimation', strList(bool2str(not ctx.field("DisableAutomaticParameterEstimation").value), nres) );
  paramStr = paramStr.replace('P_SP_a_', vec2str(ctx.field("SP_a").value, nres));
  paramStr = paramStr.replace('P_SP_A', vec2str(ctx.field("SP_capa").value, nres));
  paramStr = paramStr.replace('P_SP_alpha', vec2str(ctx.field("SP_alpha").value, nres));
  paramStr = paramStr.replace('P_FinalGridSpacingInVoxels', vec2str(ctx.field("FinalGridSpacingInVoxels").value, dim));
  
  
  templateName = ctx.field("paramFile").value;
  outfile = open(templateName,"w");
  outfile.write(paramStr);
  outfile.close();
  
  return;
  
def writeElastixParameterFile():
  fn = ctx.field("paramFile").value;
  outfile = [];
  try:
    outfile = open(fn, 'w');
    outfile.write(createElastixParameterFileText());
    outfile.close();  
  except :
    print "Error: could not open file " + fn + " for writing.";
  return;
  
def readElastixParameterFile():
  fn = ctx.field("paramFileIn").value;
  infile = [];
  lines = [];
  try:
    infile = open(fn, 'r');
    lines = infile.readlines(); 
    infile.close();  
  except :
    print "Error: could not open file " + fn + " for reading.";
  #remove comments from lines
  newlines = [];
  for l in lines:
    p = l.find("//"); 
    if p>0:
      newlines += [ l[:p] ];
    elif p < 0:
      newlines += [l];
  readElastixParameterFileFromLines(newlines);
  return;

def readElastixParameterFileFromLines(lines):
  readRegistration(lines);
  readTransform(lines);
  readMetric( lines, ctx.field("Registration").value == "MultiMetricMultiResolutionRegistration" );
  readOptimizer(lines);
  readInterpolator(lines);
  readResampleInterpolator(lines);
  readImageSampler(lines);
  readFixedImagePyramid(lines);
  readMovingImagePyramid(lines);
  readResampler(lines);
  return;

def createElastixParameterFileText():
  text = "// Parameter file created by ElastixParameterFile module\n\n"
  text += writeRegistration();
  text += writeTransform();
  text += writeMetric( ctx.field("Registration").value == "MultiMetricMultiResolutionRegistration" );
  text += writeOptimizer();
  text += writeInterpolator();
  text += writeResampleInterpolator();
  text += writeImageSampler();
  text += writeFixedImagePyramid();
  text += writeMovingImagePyramid();
  text += writeResampler();
  #print text;
  return text;

def readTransform(lines):
  numRes = ctx.field("NumberOfResolutions").value;
  imgDim = getImageDimension();
  readStringParameter(lines, ctx.field("ElastixTransform"), "Transform");
  readStringParameter(lines, ctx.field("HowToCombineTransforms"));
  if ctx.field("ElastixTransform").value == "TranslationTransform": 
    readBoolVectorParameter(lines, ctx.field("AutomaticScalesEstimation"));
    if not ctx.field("AutomaticScalesEstimation").value:
      readArrayParameter(lines, ctx.field("Scales"));
    readBoolVectorParameter(lines, ctx.field("AutomaticTransformInitialization"));
  elif ctx.field("ElastixTransform").value == "EulerTransform": 
    readBoolVectorParameter(lines, ctx.field("AutomaticScalesEstimation"));
    if not ctx.field("AutomaticScalesEstimation").value:
      readArrayParameter(lines, ctx.field("Scales"));
    readBoolVectorParameter(lines, ctx.field("AutomaticTransformInitialization"));
    ctx.field("DefineCenterOfRotation").value = readArrayParameter(lines, ctx.field("CenterOfRotation"));      
  elif ctx.field("ElastixTransform").value == "AffineTransform": 
    readBoolVectorParameter(lines, ctx.field("AutomaticScalesEstimation"));
    if not ctx.field("AutomaticScalesEstimation").value:
      readArrayParameter(lines, ctx.field("Scales"));
    readBoolVectorParameter(lines, ctx.field("AutomaticTransformInitialization"));
    ctx.field("DefineCenterOfRotation").value = readArrayParameter(lines, ctx.field("CenterOfRotation"));     
  elif ctx.field("ElastixTransform").value == "AffineDTITransform": 
    readBoolVectorParameter(lines, ctx.field("AutomaticScalesEstimation"));
    if not ctx.field("AutomaticScalesEstimation").value:
      readArrayParameter(lines, ctx.field("Scales"));
    readBoolVectorParameter(lines, ctx.field("AutomaticTransformInitialization"));
    ctx.field("DefineCenterOfRotation").value = readArrayParameter(lines, ctx.field("CenterOfRotation"));    
  elif ctx.field("ElastixTransform").value == "SimilarityTransform": 
    readBoolVectorParameter(lines, ctx.field("AutomaticScalesEstimation"));
    if not ctx.field("AutomaticScalesEstimation").value:
      readArrayParameter(lines, ctx.field("Scales"));
    readBoolVectorParameter(lines, ctx.field("AutomaticTransformInitialization"));
    ctx.field("DefineCenterOfRotation").value = readArrayParameter(lines, ctx.field("CenterOfRotation"));   
  elif ctx.field("ElastixTransform").value == "BSplineTransform": 
    ctx.field("FinalGridSpacingInVoxelsOn").value = readArrayParameter(lines, ctx.field("FinalGridSpacingInVoxels"));
    ctx.field("FinalGridSpacingInPhysicalUnitsOn").value = readArrayParameter(lines, ctx.field("FinalGridSpacingInPhysicalUnits"));
    readArrayParameter(lines, ctx.field("GridSpacingSchedule"));
    readArrayParameter(lines, ctx.field("PassiveEdgeWidth"));
    readIntParameter(lines, ctx.field("BSplineTransformSplineOrder"));
    readBoolParameter(lines, ctx.field("UseCyclicTransform"));
  elif ctx.field("ElastixTransform").value == "BSplineStackTransform": 
    ctx.field("FinalGridSpacingInVoxelsOn").value = readArrayParameter(lines, ctx.field("FinalGridSpacingInVoxels"));
    ctx.field("FinalGridSpacingInPhysicalUnitsOn").value = readArrayParameter(lines, ctx.field("FinalGridSpacingInPhysicalUnits"));
    readArrayParameter(lines, ctx.field("GridSpacingSchedule"));
    readArrayParameter(lines, ctx.field("PassiveEdgeWidth"));    
  elif ctx.field("ElastixTransform").value == "BSplineTransformWithDiffusion": 
    readArrayParameter(lines, ctx.field("FinalGridSpacingInVoxels"), "FinalGridSpacing");
    readBoolParameter(lines, ctx.field("UpsampleGridOption"));
    readIntParameter(lines, ctx.field("FilterPattern"));
    if (ctx.field("FilterPattern").value == 1):
      readIntParameter(lines, ctx.field("DiffusionEachNIterations"));
    elif (ctx.field("FilterPattern").value == 2):
      readIntVectorParameter(lines, ctx.field("AfterIterations"));
      readIntVectorParameter(lines, ctx.field("HowManyIterations"));
    readIntParameter(lines, ctx.field("NumberOfDiffusionIterations"));
    readFloatParameter(lines, ctx.field("Radius"));
    readBoolParameter(lines, ctx.field("ThresholdBool"));
    readBoolParameter(lines, ctx.field("UseFixedSegmentation"));
    readBoolParameter(lines, ctx.field("UseMovingSegmentation"));
    if (ctx.field("ThresholdBool").value):
      readFloatParameter(lines, ctx.field("ThresholdHU"));
    else :
      ctx.field("UseFixedSegmentation").value = readStringParameter(lines, ctx.field("FixedSegmentationFileName"));
      ctx.field("UseMovingSegmentation").value = readStringParameter(lines, ctx.field("MovingSegmentationFileName"));
    readBoolParameter(lines, ctx.field("WriteDiffusionFiles"));
    readBoolParameter(lines, ctx.field("GrayValueImageAlsoBasedOnFixedImage"));
    readFloatParameter(lines, ctx.field("DefaultPixelValueForGVI"));
  elif ctx.field("ElastixTransform").value == "SplineKernelTransform": 
    readStringParameter(lines, ctx.field("SplineKernelType"));
    readFloatParameter(lines, ctx.field("SplineRelaxationFactor"));
    readFloatParameter(lines, ctx.field("SplinePoissonRatio"));
  elif ctx.field("ElastixTransform").value == "WeightedCombinationTransform": 
    readBoolParameter(lines, ctx.field("NormalizeCombinationWeights"));
    readStringParameter(lines, ctx.field("SubTransforms"));
    readBoolVectorParameter(lines, ctx.field("AutomaticScalesEstimation"));
    if not ctx.field("AutomaticScalesEstimation").value:
      readIntVectorParameter(lines, ctx.field("Scales"));

def writeTransform():
  numRes = ctx.field("NumberOfResolutions").value;
  imgDim = getImageDimension();
  text = "// Transform parameters\n";
  text += stringParameterToString(ctx.field("ElastixTransform"), "Transform");
  text += stringParameterToString(ctx.field("HowToCombineTransforms"));
  if ctx.field("ElastixTransform").value == "TranslationTransform": 
    text += boolVectorParameterToString(ctx.field("AutomaticScalesEstimation"), numRes);
    if not ctx.field("AutomaticScalesEstimation").value:
      text += parameterToString(ctx.field("Scales"));
    text += boolParameterToString(ctx.field("AutomaticTransformInitialization"));
  elif ctx.field("ElastixTransform").value == "EulerTransform": 
    text += boolVectorParameterToString(ctx.field("AutomaticScalesEstimation"), numRes);
    if not ctx.field("AutomaticScalesEstimation").value:
      text += parameterToString(ctx.field("Scales"));
    text += boolParameterToString(ctx.field("AutomaticTransformInitialization"));
    if ctx.field("DefineCenterOfRotation").value:
      text += arrayParameterToString(ctx.field("CenterOfRotation"), imgDim);
  elif ctx.field("ElastixTransform").value == "AffineTransform": 
    text += boolVectorParameterToString(ctx.field("AutomaticScalesEstimation"), numRes);
    if not ctx.field("AutomaticScalesEstimation").value:
      text += parameterToString(ctx.field("Scales"));
    text += boolParameterToString(ctx.field("AutomaticTransformInitialization"));
    if ctx.field("DefineCenterOfRotation").value:
      text += arrayParameterToString(ctx.field("CenterOfRotation"), imgDim);
  elif ctx.field("ElastixTransform").value == "AffineDTITransform": 
    text += boolVectorParameterToString(ctx.field("AutomaticScalesEstimation"), numRes);
    if not ctx.field("AutomaticScalesEstimation").value:
      text += parameterToString(ctx.field("Scales"));
    text += boolParameterToString(ctx.field("AutomaticTransformInitialization"));
    if ctx.field("DefineCenterOfRotation").value:
      text += arrayParameterToString(ctx.field("CenterOfRotation"), imgDim);
  elif ctx.field("ElastixTransform").value == "SimilarityTransform": 
    text += boolVectorParameterToString(ctx.field("AutomaticScalesEstimation"), numRes);
    if not ctx.field("AutomaticScalesEstimation").value:
      text += parameterToString(ctx.field("Scales"));
    text += boolParameterToString(ctx.field("AutomaticTransformInitialization"));
    if ctx.field("DefineCenterOfRotation").value:
      text += arrayParameterToString(ctx.field("CenterOfRotation"), imgDim);
  elif ctx.field("ElastixTransform").value == "BSplineTransform": 
    if ctx.field("FinalGridSpacingInVoxelsOn").value:
      text += parameterToString(ctx.field("FinalGridSpacingInVoxels"));
    if ctx.field("FinalGridSpacingInPhysicalUnitsOn").value:
      text += parameterToString(ctx.field("FinalGridSpacingInPhysicalUnits"));
    text += arrayParameterToString(ctx.field("GridSpacingSchedule"), numRes*imgDim);
    text += arrayParameterToString(ctx.field("PassiveEdgeWidth"), numRes);
    text += parameterToString(ctx.field("BSplineTransformSplineOrder"));
    text += boolParameterToString(ctx.field("UseCyclicTransform"));
  elif ctx.field("ElastixTransform").value == "BSplineStackTransform": 
    if ctx.field("FinalGridSpacingInVoxelsOn").value:
      text += arrayParameterToString(ctx.field("FinalGridSpacingInVoxels"),imgDim);
    if ctx.field("FinalGridSpacingInPhysicalUnitsOn").value:
      text += arrayParameterToString(ctx.field("FinalGridSpacingInPhysicalUnits"),imgDim);
    text += arrayParameterToString(ctx.field("GridSpacingSchedule"), numRes*imgDim);
    text += arrayParameterToString(ctx.field("PassiveEdgeWidth"), numRes);
  elif ctx.field("ElastixTransform").value == "BSplineTransformWithDiffusion": 
    text += arrayParameterToString(ctx.field("FinalGridSpacingInVoxels"), imgDim, "FinalGridSpacing");
    text += boolParameterToString(ctx.field("UpsampleGridOption"));
    text += parameterToString(ctx.field("FilterPattern"));
    if (ctx.field("FilterPattern").value == 1):
      text += parameterToString(ctx.field("DiffusionEachNIterations"));
    elif (ctx.field("FilterPattern").value == 2):
      text += intVectorParameterToString(ctx.field("AfterIterations"), 2);
      text += intVectorParameterToString(ctx.field("HowManyIterations"), 3);
    text += parameterToString(ctx.field("NumberOfDiffusionIterations"));
    text += parameterToString(ctx.field("Radius"));
    text += boolParameterToString(ctx.field("ThresholdBool"));
    text += boolParameterToString(ctx.field("UseFixedSegmentation"));
    text += boolParameterToString(ctx.field("UseMovingSegmentation"));
    if (ctx.field("ThresholdBool").value):
      text += parameterToString(ctx.field("ThresholdHU"));
    else :
      if ctx.field("UseFixedSegmentation").value:
        text += stringParameterToString(ctx.field("FixedSegmentationFileName"));
      if ctx.field("UseMovingSegmentation").value:
        text += stringParameterToString(ctx.field("MovingSegmentationFileName"));
    text += boolParameterToString(ctx.field("WriteDiffusionFiles"));
    text += boolParameterToString(ctx.field("GrayValueImageAlsoBasedOnFixedImage"));
    text += parameterToString(ctx.field("DefaultPixelValueForGVI"));
  elif ctx.field("ElastixTransform").value == "SplineKernelTransform": 
    text += stringParameterToString(ctx.field("SplineKernelType"));
    text += parameterToString(ctx.field("SplineRelaxationFactor"));
    text += parameterToString(ctx.field("SplinePoissonRatio"));
  elif ctx.field("ElastixTransform").value == "WeightedCombinationTransform": 
    text += boolParameterToString(ctx.field("NormalizeCombinationWeights"));
    text += parameterToString(ctx.field("SubTransforms"));
    text += boolVectorParameterToString(ctx.field("AutomaticScalesEstimation"), numRes);
    if not ctx.field("AutomaticScalesEstimation").value:
      text += parameterToString(ctx.field("Scales"));
  text += "\n";
  return text;

def readRegistration(lines):
  readIntParameter(lines, ctx.field("NumberOfResolutions"));
  numRes = ctx.field("NumberOfResolutions").value;
  #text = "// Registration parameters\n";
  readStringParameter(lines, ctx.field("ElastixRegistration"), "Registration");
  readIntParameter(lines, ctx.field("FixedImageDimension"));
  readIntParameter(lines, ctx.field("MovingImageDimension"));
  readStringParameter(lines, ctx.field("FixedInternalImagePixelType"));
  readStringParameter(lines, ctx.field("MovingInternalImagePixelType"));
  readBoolParameter(lines, ctx.field("UseDirectionCosines"));
  if ctx.field("ElastixRegistration").value == "MultiMetricMultiResolutionRegistration": 
    readBoolVectorParameter(lines, ctx.field("ErodeMask"));
    ctx.field("ErodeFixedMask").value = ctx.field("ErodeMask").value;
    ctx.field("ErodeMovingMask").value = ctx.field("ErodeMask").value;
    readBoolVectorParameter(lines, ctx.field("ErodeFixedMask"));
    readBoolVectorParameter(lines, ctx.field("ErodeMovingMask"));
    metrics = readParameter(lines, "Metric");
    if len(metrics)>0:
      m = metrics[1];
      m = m.replace("\"","");
      ctx.field("Metric1").value = m;
    firstMetric = ctx.field("ElastixMetric").value;
    readMetric(lines, True);
    ctx.field("ElastixMetric").value = ctx.field("Metric1").value;
    readMetric(lines, True);
    ctx.field("ElastixMetric").value = firstMetric;
    readArrayParameter(lines, ctx.field("Metric0Weight"));
    readBoolVectorParameter(lines, ctx.field("Metric0Use"));
    readArrayParameter(lines, ctx.field("Metric1Weight"));
    readBoolVectorParameter(lines, ctx.field("Metric1Use"));
  elif ctx.field("ElastixRegistration").value == "MultiResolutionRegistration": 
    readBoolVectorParameter(lines, ctx.field("ErodeMask"));
    ctx.field("ErodeFixedMask").value = ctx.field("ErodeMask").value;
    ctx.field("ErodeMovingMask").value = ctx.field("ErodeMask").value;
    readBoolVectorParameter(lines, ctx.field("ErodeFixedMask"));
    readBoolVectorParameter(lines, ctx.field("ErodeMovingMask"));
  elif ctx.field("ElastixRegistration").value == "MultiResolutionRegistrationWithFeatures": 
    readBoolVectorParameter(lines, ctx.field("ErodeMask"));
    ctx.field("ErodeFixedMask").value = ctx.field("ErodeMask").value;
    ctx.field("ErodeMovingMask").value = ctx.field("ErodeMask").value;
    readBoolVectorParameter(lines, ctx.field("ErodeFixedMask"));
    readBoolVectorParameter(lines, ctx.field("ErodeMovingMask"));
    readArrayParameter(lines, ctx.field("Metric0Weight"));
    readArrayParameter(lines, ctx.field("Metric1Weight"));

def writeRegistration():
  numRes = ctx.field("NumberOfResolutions").value;
  text = "// Registration parameters\n";
  text += stringParameterToString(ctx.field("ElastixRegistration"), "Registration");
  text += parameterToString(ctx.field("FixedImageDimension"));
  text += parameterToString(ctx.field("MovingImageDimension"));
  text += stringParameterToString(ctx.field("FixedInternalImagePixelType"));
  text += stringParameterToString(ctx.field("MovingInternalImagePixelType"));
  text += boolParameterToString(ctx.field("UseDirectionCosines"));
  if ctx.field("ElastixRegistration").value == "MultiMetricMultiResolutionRegistration": 
    text += parameterToString(ctx.field("NumberOfResolutions"));
    text += boolVectorParameterToString(ctx.field("ErodeFixedMask"), numRes);
    text += boolVectorParameterToString(ctx.field("ErodeMovingMask"), numRes);
    text += "(Metric \"" + ctx.field("ElastixMetric").value + "\" \"" + ctx.field("Metric1").value + "\" )\n";
    firstMetric = ctx.field("ElastixMetric").value;
    text += writeMetric(True);
    ctx.field("ElastixMetric").value = ctx.field("Metric1").value;
    text += writeMetric(True);
    ctx.field("ElastixMetric").value = firstMetric;
    text += arrayParameterToString(ctx.field("Metric0Weight"), numRes);
    text += boolVectorParameterToString(ctx.field("Metric0Use"), numRes);
    text += arrayParameterToString(ctx.field("Metric1Weight"), numRes);
    text += boolVectorParameterToString(ctx.field("Metric1Use"), numRes);
  elif ctx.field("ElastixRegistration").value == "MultiResolutionRegistration": 
    text += parameterToString(ctx.field("NumberOfResolutions"));
    text += boolVectorParameterToString(ctx.field("ErodeFixedMask"), numRes);
    text += boolVectorParameterToString(ctx.field("ErodeMovingMask"), numRes);
  elif ctx.field("ElastixRegistration").value == "MultiResolutionRegistrationWithFeatures": 
    text += parameterToString(ctx.field("NumberOfResolutions"));
    text += boolVectorParameterToString(ctx.field("ErodeFixedMask"), numRes);
    text += boolVectorParameterToString(ctx.field("ErodeMovingMask"), numRes);
    text += arrayParameterToString(ctx.field("Metric0Weight"), numRes);
    text += arrayParameterToString(ctx.field("Metric1Weight"), numRes);
  text += "\n";
  return text;

def readMetric(lines, multi = False):
  numRes = ctx.field("NumberOfResolutions").value;
  text = "// Metric parameters\n";
  if (not multi):
    readStringParameter(lines, ctx.field("ElastixMetric"), "Metric");
  readBoolVectorParameter(lines, ctx.field("ShowExactMetricValues"));
  readBoolVectorParameter(lines, ctx.field("CheckNumberOfSamples"));
  readFloatParameter(lines, ctx.field("RequiredRatioOfValidSamples"));
  if ctx.field("ElastixMetric").value == "AdvancedKappaStatistic":
    readBoolVectorParameter(lines, ctx.field("AdvancedKappaStatisticUseComplement"), "UseComplement");
    readFloatParameter(lines, ctx.field("AdvancedKappaStatisticForeGroundvalue"), "ForeGroundvalue");
  elif ctx.field("ElastixMetric").value == "AdvancedMattesMutualInformation":
    readArrayParameter(lines, ctx.field("NumberOfHistogramBins"));
    readArrayParameter(lines, ctx.field("NumberOfFixedHistogramBins"));
    readArrayParameter(lines, ctx.field("FixedKernelBSplineOrder"));
    readArrayParameter(lines, ctx.field("FixedLimitRangeRatio"));
    readArrayParameter(lines, ctx.field("NumberOfMovingHistogramBins"));
    readArrayParameter(lines, ctx.field("MovingKernelBSplineOrder"));
    readArrayParameter(lines, ctx.field("MovingLimitRangeRatio"));
    readBoolParameter(lines, ctx.field("UseFastAndLowMemoryVersion"));
  elif ctx.field("ElastixMetric").value == "AdvancedMeanSquares":
    readBoolParameter(lines, ctx.field("UseNormalization"));
  elif ctx.field("ElastixMetric").value == "AdvancedNormalizedCorrelation":
    readBoolParameter(lines, ctx.field("SubtractMean"));
  elif ctx.field("ElastixMetric").value == "NormalizedMutualInformation":
    readArrayParameter(lines, ctx.field("NumberOfHistogramBins"));
    readArrayParameter(lines, ctx.field("NumberOfFixedHistogramBins"));
    readArrayParameter(lines, ctx.field("FixedKernelBSplineOrder"));
    readArrayParameter(lines, ctx.field("FixedLimitRangeRatio"));
    readArrayParameter(lines, ctx.field("NumberOfMovingHistogramBins"));
    readArrayParameter(lines, ctx.field("MovingKernelBSplineOrder"));
    readArrayParameter(lines, ctx.field("MovingLimitRangeRatio"));
    readBoolParameter(lines, ctx.field("UseFastAndLowMemoryVersion"));
  elif ctx.field("ElastixMetric").value == "VarianceOverLastDimensionMetric":
    readBoolParameter(lines, ctx.field("SampleLastDimensionRandomly"));
    readIntParameter(lines, ctx.field("NumSamplesLastDimension"));
    readBoolParameter(lines, ctx.field("SubtractMean"));
    readFloatParameter(lines, ctx.field("MovingImageDerivativeScales"));

def writeMetric(multi = False):
  numRes = ctx.field("NumberOfResolutions").value;
  text = "// Metric parameters\n";
  if (not multi):
    text += stringParameterToString(ctx.field("ElastixMetric"), "Metric");
  text += boolVectorParameterToString(ctx.field("ShowExactMetricValues"), numRes);
  text += boolVectorParameterToString(ctx.field("CheckNumberOfSamples"), numRes);
  text += parameterToString(ctx.field("RequiredRatioOfValidSamples"));
  if ctx.field("ElastixMetric").value == "AdvancedKappaStatistic":
    text += boolVectorParameterToString(ctx.field("AdvancedKappaStatisticUseComplement"), numRes, "UseComplement");
    text += parameterToString(ctx.field("AdvancedKappaStatisticForeGroundvalue"), "ForeGroundvalue");
  elif ctx.field("ElastixMetric").value == "AdvancedMattesMutualInformation":
    text += arrayParameterToString(ctx.field("NumberOfHistogramBins"), numRes);
    text += arrayParameterToString(ctx.field("NumberOfFixedHistogramBins"), numRes);
    text += arrayParameterToString(ctx.field("FixedKernelBSplineOrder"), numRes);
    text += arrayParameterToString(ctx.field("FixedLimitRangeRatio"), numRes);
    text += arrayParameterToString(ctx.field("NumberOfMovingHistogramBins"), numRes);
    text += arrayParameterToString(ctx.field("MovingKernelBSplineOrder"), numRes);
    text += arrayParameterToString(ctx.field("MovingLimitRangeRatio"), numRes);
    text += boolParameterToString(ctx.field("UseFastAndLowMemoryVersion"));
  elif ctx.field("ElastixMetric").value == "AdvancedMeanSquares":
    text += boolParameterToString(ctx.field("UseNormalization"));
  elif ctx.field("ElastixMetric").value == "AdvancedNormalizedCorrelation":
    text += boolParameterToString(ctx.field("SubtractMean"));
  elif ctx.field("ElastixMetric").value == "NormalizedMutualInformation":
    text += arrayParameterToString(ctx.field("NumberOfHistogramBins"), numRes);
    text += arrayParameterToString(ctx.field("NumberOfFixedHistogramBins"), numRes);
    text += arrayParameterToString(ctx.field("FixedKernelBSplineOrder"), numRes);
    text += arrayParameterToString(ctx.field("FixedLimitRangeRatio"), numRes);
    text += arrayParameterToString(ctx.field("NumberOfMovingHistogramBins"), numRes);
    text += arrayParameterToString(ctx.field("MovingKernelBSplineOrder"), numRes);
    text += arrayParameterToString(ctx.field("MovingLimitRangeRatio"), numRes);
    text += boolParameterToString(ctx.field("UseFastAndLowMemoryVersion"));
  elif ctx.field("ElastixMetric").value == "VarianceOverLastDimensionMetric":
    text += boolParameterToString(ctx.field("SampleLastDimensionRandomly"));
    text += parameterToString(ctx.field("NumSamplesLastDimension"));
    text += boolParameterToString(ctx.field("SubtractMean"));
    text += parameterToString(ctx.field("MovingImageDerivativeScales"));
  text += "\n";
  return text;

def readInterpolator(lines):
  readStringParameter(lines, ctx.field("ElastixInterpolator"), "Interpolator");
  if ctx.field("ElastixInterpolator").value == "NearestNeighborInterpolator":
    return;
  elif ctx.field("ElastixInterpolator").value == "LinearInterpolator":
    return;
  elif ctx.field("ElastixInterpolator").value == "BSplineInterpolator":
    readArrayParameter(lines, ctx.field("BSplineInterpolationOrder"));
  elif ctx.field("ElastixInterpolator").value == "BSplineInterpolatorFloat":
    readArrayParameter(lines, ctx.field("BSplineInterpolationOrder"));
  elif ctx.field("ElastixInterpolator").value == "ReducedDimensionBSplineInterpolator":
    readArrayParameter(lines, ctx.field("BSplineInterpolationOrder"));

def writeInterpolator():
  numRes = ctx.field("NumberOfResolutions").value;
  text = "// Interpolator parameters\n";
  text += stringParameterToString(ctx.field("ElastixInterpolator"), "Interpolator");
  if ctx.field("ElastixInterpolator").value == "NearestNeighborInterpolator":
    text += "";
  elif ctx.field("ElastixInterpolator").value == "LinearInterpolator":
    text += "";
  elif ctx.field("ElastixInterpolator").value == "BSplineInterpolator":
    text += arrayParameterToString(ctx.field("BSplineInterpolationOrder"), numRes);
  elif ctx.field("ElastixInterpolator").value == "BSplineInterpolatorFloat":
    text += arrayParameterToString(ctx.field("BSplineInterpolationOrder"), numRes);
  elif ctx.field("ElastixInterpolator").value == "ReducedDimensionBSplineInterpolator":
    text += arrayParameterToString(ctx.field("BSplineInterpolationOrder"), numRes);
  text += "\n";
  return text;

def writeResampleInterpolator():
  numRes = ctx.field("NumberOfResolutions").value;
  text = "// ResampleInterpolator parameters\n";
  text += stringParameterToString(ctx.field("ElastixResampleInterpolator"), "ResampleInterpolator");
  if ctx.field("ElastixResampleInterpolator").value == "FinalNearestNeighborInterpolator":
    text += "";
  elif ctx.field("ElastixResampleInterpolator").value == "FinalLinearInterpolator":
    text += "";
  elif ctx.field("ElastixResampleInterpolator").value == "FinalBSplineInterpolator":
    text += parameterToString(ctx.field("FinalBSplineInterpolationOrder"));
  elif ctx.field("ElastixResampleInterpolator").value == "FinalBSplineInterpolatorFloat":
    text += parameterToString(ctx.field("FinalBSplineInterpolationOrder"));
  text += "\n";
  return text;

def readResampleInterpolator(lines):
  numRes = ctx.field("NumberOfResolutions").value;
  readStringParameter(lines, ctx.field("ElastixResampleInterpolator"), "ResampleInterpolator");  

def readImageSampler(lines):
  readStringParameter(lines, ctx.field("ElastixImageSampler"), "ImageSampler");
  if ctx.field("ElastixImageSampler").value == "Full":
    return;
  elif ctx.field("ElastixImageSampler").value == "Grid":
    readArrayParameter(lines, ctx.field("SampleGridSpacing"));
  elif ctx.field("ElastixImageSampler").value == "MultiInputRandomCoordinate":
    readArrayParameter(lines, ctx.field("NumberOfSpatialSamples"));
    readBoolParameter(lines, ctx.field("UseRandomSampleRegion"));
    ctx.field("UseRandomSampleRegion").value = readArrayParameter(lines, ctx.field("SampleRegionSize"));
    readIntVectorParameter(lines, ctx.field("FixedImageBSplineInterpolationOrder"));
  elif ctx.field("ElastixImageSampler").value == "RandomCoordinate":
    readIntVectorParameter(lines, ctx.field("NumberOfSpatialSamples"));
    readBoolParameter(lines, ctx.field("UseRandomSampleRegion"));
    ctx.field("UseRandomSampleRegion").value = readArrayParameter(lines, ctx.field("SampleRegionSize"));
    readIntVectorParameter(lines, ctx.field("FixedImageBSplineInterpolationOrder"));
  elif ctx.field("ElastixImageSampler").value == "Random":
    readIntVectorParameter(lines, ctx.field("NumberOfSpatialSamples"));
  elif ctx.field("ElastixImageSampler").value == "RandomSparseMask":
    readIntVectorParameter(lines, ctx.field("NumberOfSpatialSamples"));

def writeImageSampler():
  numRes = ctx.field("NumberOfResolutions").value;
  imgDim = getImageDimension();
  text = "// ImageSampler parameters\n";
  text += stringParameterToString(ctx.field("ElastixImageSampler"), "ImageSampler");
  if ctx.field("ElastixImageSampler").value == "Full":
    text += "";
  elif ctx.field("ElastixImageSampler").value == "Grid":
    text += arrayParameterToString(ctx.field("SampleGridSpacing"),imgDim);
  elif ctx.field("ElastixImageSampler").value == "MultiInputRandomCoordinate":
    text += arrayParameterToString(ctx.field("NumberOfSpatialSamples"),numRes);
    text += boolParameterToString(ctx.field("UseRandomSampleRegion"));
    if ctx.field("UseRandomSampleRegion").value:
      text += parameterToString(ctx.field("SampleRegionSize"));
    text += arrayParameterToString(ctx.field("FixedImageBSplineInterpolationOrder"),numRes);
  elif ctx.field("ElastixImageSampler").value == "RandomCoordinate":
    text += arrayParameterToString(ctx.field("NumberOfSpatialSamples"),numRes);
    text += boolParameterToString(ctx.field("UseRandomSampleRegion"));
    if ctx.field("UseRandomSampleRegion").value:
      text += parameterToString(ctx.field("SampleRegionSize"));
    text += arrayParameterToString(ctx.field("FixedImageBSplineInterpolationOrder"),numRes);
  elif ctx.field("ElastixImageSampler").value == "Random":
    text += arrayParameterToString(ctx.field("NumberOfSpatialSamples"),numRes);
  elif ctx.field("ElastixImageSampler").value == "RandomSparseMask":
    text += arrayParameterToString(ctx.field("NumberOfSpatialSamples"),numRes);
  text += "\n";
  return text;

def readFixedImagePyramid(lines):
  readStringParameter(lines, ctx.field("ElastixFixedImagePyramid"), "FixedImagePyramid");
  if ctx.field("ElastixFixedImagePyramid").value == "FixedRecursiveImagePyramid":
    ctx.field("DefineFixedImagePyramidSchedule").value = readIntVectorParameter(lines, ctx.field("FixedImagePyramidSchedule"));
  elif ctx.field("ElastixFixedImagePyramid").value == "FixedSmoothingImagePyramid":
    ctx.field("DefineFixedImagePyramidSchedule").value = readIntVectorParameter(lines, ctx.field("FixedImagePyramidSchedule"));
  elif ctx.field("ElastixFixedImagePyramid").value == "FixedShrinkingImagePyramid":
    ctx.field("DefineFixedImagePyramidSchedule").value = readIntVectorParameter(lines, ctx.field("FixedImagePyramidSchedule"));

def writeFixedImagePyramid():
  numRes = ctx.field("NumberOfResolutions").value;
  imgDim = getImageDimension();
  text = "// FixedImagePyramid parameters\n";
  text += stringParameterToString(ctx.field("ElastixFixedImagePyramid"), "FixedImagePyramid");
  if ctx.field("ElastixFixedImagePyramid").value == "FixedRecursiveImagePyramid":
    if ctx.field("DefineFixedImagePyramidSchedule").value:
      text += arrayParameterToString(ctx.field("FixedImagePyramidSchedule"), numRes*imgDim);
  elif ctx.field("ElastixFixedImagePyramid").value == "FixedSmoothingImagePyramid":
    if ctx.field("DefineFixedImagePyramidSchedule").value:
      text += arrayParameterToString(ctx.field("FixedImagePyramidSchedule"), numRes*imgDim);
  elif ctx.field("ElastixFixedImagePyramid").value == "FixedShrinkingImagePyramid":
    if ctx.field("DefineFixedImagePyramidSchedule").value:
      text += arrayParameterToString(ctx.field("FixedImagePyramidSchedule"), numRes*imgDim); 
  text += "\n";
  return text;

def readMovingImagePyramid(lines):
  readStringParameter(lines, ctx.field("ElastixMovingImagePyramid"), "MovingImagePyramid");
  if ctx.field("ElastixMovingImagePyramid").value == "MovingRecursiveImagePyramid":
    ctx.field("DefineMovingImagePyramidSchedule").value = readIntVectorParameter(lines, ctx.field("MovingImagePyramidSchedule"));
  elif ctx.field("ElastixMovingImagePyramid").value == "MovingSmoothingImagePyramid":
    ctx.field("DefineMovingImagePyramidSchedule").value = readIntVectorParameter(lines, ctx.field("MovingImagePyramidSchedule"));
  elif ctx.field("ElastixMovingImagePyramid").value == "MovingShrinkingImagePyramid":
    ctx.field("DefineMovingImagePyramidSchedule").value = readIntVectorParameter(lines, ctx.field("MovingImagePyramidSchedule"));

def writeMovingImagePyramid():
  numRes = ctx.field("NumberOfResolutions").value;
  imgDim = getImageDimension();
  text = "// MovingImagePyramid parameters\n";
  if ctx.field("ElastixMovingImagePyramid").value == "AsFixed":
    text = writeFixedImagePyramid();
    text = text.replace("Fixed", "Moving");
  else :
    text += stringParameterToString(ctx.field("ElastixMovingImagePyramid"), "MovingImagePyramid");
    if ctx.field("ElastixMovingImagePyramid").value == "MovingRecursiveImagePyramid":
      if ctx.field("DefineMovingImagePyramidSchedule").value:
        text += arrayParameterToString(ctx.field("MovingImagePyramidSchedule"), numRes*imgDim);
    elif ctx.field("ElastixMovingImagePyramid").value == "MovingSmoothingImagePyramid":
      if ctx.field("DefineMovingImagePyramidSchedule").value:
        text += arrayParameterToString(ctx.field("MovingImagePyramidSchedule"), numRes*imgDim);
    elif ctx.field("ElastixMovingImagePyramid").value == "MovingShrinkingImagePyramid":
      if ctx.field("DefineMovingImagePyramidSchedule").value:
        text += arrayParameterToString(ctx.field("MovingImagePyramidSchedule"), numRes*imgDim);
    text += "\n";
  text += boolParameterToString(ctx.field("WritePyramidImagesAfterEachResolution"));
  text += "\n";  
  return text;

def readOptimizer(lines):
  numRes = ctx.field("NumberOfResolutions").value;
  imgDim = getImageDimension();
  readStringParameter(lines, ctx.field("ElastixOptimizer"), "Optimizer");
  readBoolParameter(lines, ctx.field("NewSamplesEveryIteration"));
  if ctx.field("ElastixOptimizer").value == "AdaptiveStochasticGradientDescent":
    readIntVectorParameter(lines, ctx.field("MaximumNumberOfIterations"));
    readIntVectorParameter(lines, ctx.field("MaximumNumberOfSamplingAttempts"));
    readBoolParameter(lines, ctx.field("AutomaticParameterEstimation"));
    if (ctx.field("AutomaticParameterEstimation").value):
      ctx.field("DefineNumberOfGradientMeasurements").value = readIntVectorParameter(lines, ctx.field("NumberOfGradientMeasurements"));
    if (ctx.field("AutomaticParameterEstimation").value):
      ctx.field("DefineNumberOfJacobianMeasurements").value = readIntVectorParameter(lines, ctx.field("NumberOfJacobianMeasurements"));
    if (not ctx.field("AutomaticParameterEstimation").value):
      readArrayParameter(lines, ctx.field("SP_a"));
      readArrayParameter(lines, ctx.field("SP_capa"), "SP_A");
      readArrayParameter(lines, ctx.field("SP_alpha"));
      readArrayParameter(lines, ctx.field("SigmoidMin"));
      readArrayParameter(ctx.field("SigmoidMax"));
      readArrayParameter(ctx.field("SigmoidScale"));
    readArrayParameter(lines, ctx.field("SigmoidInitialTime"));
    readBoolParameter(lines, ctx.field("UseAdaptiveStepSizes"));
    ctx.field("DefineMinimumStepLength").value = readArrayParameter(lines, ctx.field("MaximumStepLength"));
    readIntParameter(lines, ctx.field("NumberOfSamplesForExactGradient"));
  elif ctx.field("ElastixOptimizer").value == "CMAEvolutionStrategy":
    readIntVectorParameter(lines, ctx.field("MaximumNumberOfIterations"));
    readArrayParameter(lines, ctx.field("StepLength"));
    readArrayParameter(lines, ctx.field("ValueTolerance"));
    readArrayParameter(lines, ctx.field("PositionToleranceMin"));
    readArrayParameter(lines, ctx.field("PositionToleranceMax"));
    ctx.field("DefineMinimumDeviation").value = readArrayParameter(lines, ctx.field("MinimumDeviation"));
    ctx.field("DefineMaximumDeviation").value = readArrayParameter(lines, ctx.field("MaximumDeviation"));
    readArrayParameter(lines, ctx.field("PopulationSize"));
    readArrayParameter(lines, ctx.field("NumberOfParents"));
    readBoolParameter(lines, ctx.field("UseDecayingSigma"));
    if (ctx.field("UseDecayingSigma").value):
      readArrayParameter(lines, ctx.field("SP_capa"), "SP_A");
      readArrayParameter(lines, ctx.field("SP_alpha"));
    readBoolParameter(lines, ctx.field("UseCovarianceMatrixAdaptation"));
    readStringParameter(lines, ctx.field("RecombinationWeightsPreset"));
    readBoolParameter(lines, ctx.field("UpdateBDPeriod"));
  elif ctx.field("ElastixOptimizer").value == "ConjugateGradient":
    readArrayParameter(lines, ctx.field("MaximumNumberOfIterations"));
    readBoolParameter(lines, ctx.field("GenerateLineSearchIterations"));
    readArrayParameter(lines, ctx.field("MaximumNumberOfLineSearchIterations"));
    readArrayParameter(lines, ctx.field("StepLength"));
    readArrayParameter(lines, ctx.field("LineSearchValueTolerance"));
    readArrayParameter(lines, ctx.field("LineSearchGradientTolerance"));
    readArrayParameter(lines, ctx.field("ValueTolerance"));
    readArrayParameter(lines, ctx.field("GradientMagnitudeTolerance"));
    readStringParameter(lines, ctx.field("ConjugateGradientType"));
    readBoolParameter(lines, ctx.field("StopIfWolfeNotSatisfied"));    
  elif ctx.field("ElastixOptimizer").value == "ConjugateGradientFRPR":
    readArrayParameter(lines, ctx.field("MaximumNumberOfIterations"));
    readArrayParameter(lines, ctx.field("MaximumNumberOfLineSearchIterations"));
    readArrayParameter(lines, ctx.field("StepLength"));
    readArrayParameter(lines, ctx.field("ValueTolerance"));
    readArrayParameter(lines, ctx.field("LineSearchStepTolerance"));
  elif ctx.field("ElastixOptimizer").value == "FiniteDifferenceGradientDescent":
    readArrayParameter(lines, ctx.field("MaximumNumberOfIterations"));
    readArrayParameter(lines, ctx.field("SP_a"));
    readArrayParameter(lines, ctx.field("SP_capa"), "SP_A");
    readArrayParameter(lines, ctx.field("SP_alpha"));
    readArrayParameter(lines, ctx.field("SP_c"));
    readArrayParameter(lines, ctx.field("SP_gamma"));
    readBoolParameter(lines, ctx.field("ShowMetricValues"));
  elif ctx.field("ElastixOptimizer").value == "FullSearch":
    readTextLineParameter(lines, ctx.field("FullSearchSpace"));
  elif ctx.field("ElastixOptimizer").value == "QuasiNewtonLBFGS":  
    readArrayParameter(lines, ctx.field("MaximumNumberOfIterations"));
    readBoolParameter(lines, ctx.field("GenerateLineSearchIterations"));
    readIntVectorParameter(lines, ctx.field("MaximumNumberOfLineSearchIterations"));
    readArrayParameter(lines, ctx.field("StepLength"));
    readArrayParameter(lines, ctx.field("LineSearchValueTolerance"));
    readArrayParameter(lines, ctx.field("LineSearchGradientTolerance"));
    readArrayParameter(lines, ctx.field("ValueTolerance"));
    readArrayParameter(lines, ctx.field("GradientMagnitudeTolerance"));
    readArrayParameter(lines, ctx.field("LBFGSUpdateAccuracy"));
    readBoolParameter(lines, ctx.field("StopIfWolfeNotSatisfied"));    
  elif ctx.field("ElastixOptimizer").value == "RegularStepGradientDescent":
    readArrayParameter(lines, ctx.field("MaximumNumberOfIterations"));
    readArrayParameter(lines, ctx.field("MinimumGradientMagnitude"));
    ctx.field("DefineMinimumStepLength").value = readArrayParameter(lines, ctx.field("MinimumStepLength"));
    ctx.field("DefineMaximumStepLength").value = readArrayParameter(lines, ctx.field("RSGDMaximumStepLength"));
    readArrayParameter(lines, ctx.field("RelaxationFactor"));
  elif ctx.field("ElastixOptimizer").value == "RSGDEachParameterApart":
    readArrayParameter(lines, ctx.field("MaximumNumberOfIterations"));
    readArrayParameter(lines, ctx.field("MinimumGradientMagnitude"));
    ctx.field("DefineMinimumStepLength").value = readArrayParameter(lines, ctx.field("MinimumStepLength"));
    ctx.field("DefineMaximumStepLength").value = readArrayParameter(lines, ctx.field("RSGDMaximumStepLength"));
  elif ctx.field("ElastixOptimizer").value == "SimultaneousPerturbation":
    readArrayParameter(lines, ctx.field("MaximumNumberOfIterations"));
    readArrayParameter(lines, ctx.field("NumberOfPerturbations"));
    readArrayParameter(lines, ctx.field("SP_a"));
    readArrayParameter(lines, ctx.field("SP_capa"), "SP_A");
    readArrayParameter(lines, ctx.field("SP_alpha"));
    readArrayParameter(lines, ctx.field("SP_c"));
    readArrayParameter(lines, ctx.field("SP_gamma"));
    readBoolParameter(lines, ctx.field("ShowMetricValues"));  
  elif ctx.field("ElastixOptimizer").value == "StandardGradientDescent":
    readArrayParameter(lines, ctx.field("MaximumNumberOfIterations"));
    readArrayParameter(lines, ctx.field("MaximumNumberOfSamplingAttempts"));
    readArrayParameter(lines, ctx.field("SP_a"));
    readArrayParameter(lines, ctx.field("SP_capa"), "SP_A");
    readArrayParameter(lines, ctx.field("SP_alpha"));

def writeOptimizer():
  numRes = ctx.field("NumberOfResolutions").value;
  imgDim = getImageDimension();
  text = "// Optimizer parameters\n";
  text += stringParameterToString(ctx.field("ElastixOptimizer"), "Optimizer");
  text += boolParameterToString(ctx.field("NewSamplesEveryIteration"));
  if ctx.field("ElastixOptimizer").value == "AdaptiveStochasticGradientDescent":
    text += arrayParameterToString(ctx.field("MaximumNumberOfIterations"), numRes);
    text += arrayParameterToString(ctx.field("MaximumNumberOfSamplingAttempts"), numRes);
    text += boolParameterToString(ctx.field("AutomaticParameterEstimation"));
    if (ctx.field("AutomaticParameterEstimation").value and ctx.field("DefineNumberOfGradientMeasurements").value):
      text += intVectorParameterToString(ctx.field("NumberOfGradientMeasurements"), numRes);
    if (ctx.field("AutomaticParameterEstimation").value and ctx.field("DefineNumberOfJacobianMeasurements").value):
      text += arrayParameterToString(ctx.field("NumberOfJacobianMeasurements"), numRes);
    if (not ctx.field("AutomaticParameterEstimation").value):
      text += arrayParameterToString(ctx.field("SP_a"), numRes);
      text += arrayParameterToString(ctx.field("SP_capa"), numRes, "SP_A");
      text += arrayParameterToString(ctx.field("SP_alpha"), numRes);
      text += arrayParameterToString(ctx.field("SigmoidMin"), numRes);
      text += arrayParameterToString(ctx.field("SigmoidMax"), numRes);
      text += arrayParameterToString(ctx.field("SigmoidScale"), numRes);
    text += arrayParameterToString(ctx.field("SigmoidInitialTime"), numRes);
    text += boolParameterToString(ctx.field("UseAdaptiveStepSizes"));
    if ctx.field("DefineMinimumStepLength").value:
      text += arrayParameterToString(ctx.field("MaximumStepLength"), numRes);
    text += parameterToString(ctx.field("NumberOfSamplesForExactGradient"));
  elif ctx.field("ElastixOptimizer").value == "CMAEvolutionStrategy":
    text += arrayParameterToString(ctx.field("MaximumNumberOfIterations"), numRes);
    text += arrayParameterToString(ctx.field("StepLength"), numRes);
    text += arrayParameterToString(ctx.field("ValueTolerance"), numRes);
    text += arrayParameterToString(ctx.field("PositionToleranceMin"), numRes);
    text += arrayParameterToString(ctx.field("PositionToleranceMax"), numRes);
    if ctx.field("DefineMinimumDeviation").value:
      text += arrayParameterToString(ctx.field("MinimumDeviation"), numRes);
    if ctx.field("DefineMaximumDeviation").value:
      text += arrayParameterToString(ctx.field("MaximumDeviation"), numRes);
    text += arrayParameterToString(ctx.field("PopulationSize"), numRes);
    text += arrayParameterToString(ctx.field("NumberOfParents"), numRes);
    text += boolParameterToString(ctx.field("UseDecayingSigma"));
    if (ctx.field("UseDecayingSigma").value):
      text += arrayParameterToString(ctx.field("SP_capa"), numRes, "SP_A");
      text += arrayParameterToString(ctx.field("SP_alpha"), numRes);
    text += boolParameterToString(ctx.field("UseCovarianceMatrixAdaptation"));
    text += stringParameterToString(ctx.field("RecombinationWeightsPreset"));
    text += boolParameterToString(ctx.field("UpdateBDPeriod"));
  elif ctx.field("ElastixOptimizer").value == "ConjugateGradient":
    text += arrayParameterToString(ctx.field("MaximumNumberOfIterations"), numRes);
    text += boolParameterToString(ctx.field("GenerateLineSearchIterations"));
    text += arrayParameterToString(ctx.field("MaximumNumberOfLineSearchIterations"), numRes);
    text += arrayParameterToString(ctx.field("StepLength"), numRes);
    text += arrayParameterToString(ctx.field("LineSearchValueTolerance"), numRes);
    text += arrayParameterToString(ctx.field("LineSearchGradientTolerance"), numRes);
    text += arrayParameterToString(ctx.field("ValueTolerance"), numRes);
    text += arrayParameterToString(ctx.field("GradientMagnitudeTolerance"), numRes);
    text += stringParameterToString(ctx.field("ConjugateGradientType"));
    text += boolParameterToString(ctx.field("StopIfWolfeNotSatisfied"));    
  elif ctx.field("ElastixOptimizer").value == "ConjugateGradientFRPR":
    text += arrayParameterToString(ctx.field("MaximumNumberOfIterations"), numRes);
    text += arrayParameterToString(ctx.field("MaximumNumberOfLineSearchIterations"), numRes);
    text += arrayParameterToString(ctx.field("StepLength"), numRes);
    text += arrayParameterToString(ctx.field("ValueTolerance"), numRes);
    text += arrayParameterToString(ctx.field("LineSearchStepTolerance"), numRes);
  elif ctx.field("ElastixOptimizer").value == "FiniteDifferenceGradientDescent":
    text += arrayParameterToString(ctx.field("MaximumNumberOfIterations"), numRes);
    text += arrayParameterToString(ctx.field("SP_a"), numRes);
    text += arrayParameterToString(ctx.field("SP_capa"), numRes, "SP_A");
    text += arrayParameterToString(ctx.field("SP_alpha"), numRes);
    text += arrayParameterToString(ctx.field("SP_c"), numRes);
    text += arrayParameterToString(ctx.field("SP_gamma"), numRes);
    text += boolParameterToString(ctx.field("ShowMetricValues"));
  elif ctx.field("ElastixOptimizer").value == "FullSearch":
    text += ctx.field("FullSearchSpace").value + "\n";
  elif ctx.field("ElastixOptimizer").value == "QuasiNewtonLBFGS":  
    text += arrayParameterToString(ctx.field("MaximumNumberOfIterations"), numRes);
    text += boolParameterToString(ctx.field("GenerateLineSearchIterations"));
    text += intVectorParameterToString(ctx.field("MaximumNumberOfLineSearchIterations"), numRes);
    text += arrayParameterToString(ctx.field("StepLength"), numRes);
    text += arrayParameterToString(ctx.field("LineSearchValueTolerance"), numRes);
    text += arrayParameterToString(ctx.field("LineSearchGradientTolerance"), numRes);
    text += arrayParameterToString(ctx.field("ValueTolerance"), numRes);
    text += arrayParameterToString(ctx.field("GradientMagnitudeTolerance"), numRes);
    text += arrayParameterToString(ctx.field("LBFGSUpdateAccuracy"), numRes);
    text += boolParameterToString(ctx.field("StopIfWolfeNotSatisfied"));    
  elif ctx.field("ElastixOptimizer").value == "RegularStepGradientDescent":
    text += arrayParameterToString(ctx.field("MaximumNumberOfIterations"), numRes);
    text += arrayParameterToString(ctx.field("MinimumGradientMagnitude"), numRes);
    if ctx.field("DefineMinimumStepLength").value:
      text += arrayParameterToString(ctx.field("MinimumStepLength"), numRes);
    if ctx.field("DefineMaximumStepLength").value:
      text += arrayParameterToString(ctx.field("RSGDMaximumStepLength"), numRes);
    text += arrayParameterToString(ctx.field("RelaxationFactor"), numRes);
  elif ctx.field("ElastixOptimizer").value == "RSGDEachParameterApart":
    text += arrayParameterToString(ctx.field("MaximumNumberOfIterations"), numRes);
    text += arrayParameterToString(ctx.field("MinimumGradientMagnitude"), numRes);
    if ctx.field("DefineMinimumStepLength").value:
      text += arrayParameterToString(ctx.field("MinimumStepLength"), numRes);
    if ctx.field("DefineMaximumStepLength").value:
      text += arrayParameterToString(ctx.field("RSGDMaximumStepLength"), numRes);
  elif ctx.field("ElastixOptimizer").value == "SimultaneousPerturbation":
    text += arrayParameterToString(ctx.field("MaximumNumberOfIterations"), numRes);
    text += arrayParameterToString(ctx.field("NumberOfPerturbations"), numRes);
    text += arrayParameterToString(ctx.field("SP_a"), numRes);
    text += arrayParameterToString(ctx.field("SP_capa"), numRes, "SP_A");
    text += arrayParameterToString(ctx.field("SP_alpha"), numRes);
    text += arrayParameterToString(ctx.field("SP_c"), numRes);
    text += arrayParameterToString(ctx.field("SP_gamma"), numRes);
    text += boolParameterToString(ctx.field("ShowMetricValues"));  
  elif ctx.field("ElastixOptimizer").value == "StandardGradientDescent":
    text += arrayParameterToString(ctx.field("MaximumNumberOfIterations"), numRes);
    text += arrayParameterToString(ctx.field("MaximumNumberOfSamplingAttempts"), numRes);
    text += arrayParameterToString(ctx.field("SP_a"), numRes);
    text += arrayParameterToString(ctx.field("SP_capa"), numRes, "SP_A");
    text += arrayParameterToString(ctx.field("SP_alpha"), numRes); 
  text += "\n";
  return text;

def readResampler(lines):
  readStringParameter(lines, ctx.field("ElastixResampler"), "Resampler");
  readBoolParameter(lines, ctx.field("WriteResultImage"));
  readBoolParameter(lines, ctx.field("CompressResultImage"));
  readStringParameter(lines, ctx.field("ResultImageFormat"));
  readStringParameter(lines, ctx.field("ResultImagePixelType"));
  readBoolParameter(lines, ctx.field("WriteResultImageAfterEachResolution"));
  
def writeResampler():
  numRes = ctx.field("NumberOfResolutions").value;
  imgDim = getImageDimension();
  text = "// Resampler parameters\n";
  text += stringParameterToString(ctx.field("ElastixResampler"), "Resampler");
  text += boolParameterToString(ctx.field("WriteResultImage"));
  text += boolParameterToString(ctx.field("CompressResultImage"));
  text += stringParameterToString(ctx.field("ResultImageFormat"));
  text += stringParameterToString(ctx.field("ResultImagePixelType"));
  text += boolParameterToString(ctx.field("WriteResultImageAfterEachResolution"));
  text += "\n";
  return text;
  
def getImageDimension():
  return ctx.field("FixedImageDimension").value;

def parameterToString(f, name=""):
  if len(name)==0:
    text = "(" + str( f.getName() ) + " " + str(f.value) + ")\n";
  else :
    text = "(" + name + " " + str(f.value) + ")\n";
  return text;

def readIntParameter(lines, f, name=""):
  n = name;
  if len(name)==0:
    n = str( f.getName() );
  for l in lines:
    if l.find(n) >= 0:
      s = l.replace("(", "");
      s = s.replace(")", "");
      ts = s.split();
      ret = ts[1];
      print str(f.getName()) + ": " + ret;
      f.setValue( int(ret) );
      return True;
  return False;

def readFloatParameter(lines, f, name=""):
  n = name;
  if len(name)==0:
    n = str( f.getName() );
  for l in lines:
    if l.find(n) >= 0:
      s = l.replace("(", "");
      s = s.replace(")", "");
      ts = s.split();
      ret = ts[1];
      print str(f.getName()) + ": " + ret;
      f.setValue( float(ret) );
      return True;
  return False;

def readStringParameter(lines, f, name=""):
  n = name;
  if len(name)==0:
    n = str( f.getName() );
  for l in lines:
    if l.find(n) >= 0:
      s = l.replace("(", "");
      s = s.replace(")", "");
      ts = s.split();
      ret = ts[1].replace("\"", "");
      print str(f.getName()) + ": " + ret;
      f.setValue( ret );
      return True;
  return False;

def readParameter(lines, name):
  for l in lines:
    if l.find(name) >= 0:
      s = l.replace("(", "");
      s = s.replace(")", "");
      ts = s.split();
      return ts[1:];
  return [];

def readTextLineParameter(lines, f, name=""):
  n = name;
  if len(name)==0:
    n = str( f.getName() );
  s = "";
  for l in lines:
    if l.find(n) >= 0:
      s += l + "\n";
  if len(s) > 0:
    print str(f.getName()) + ": " + s;
    f.setValue(s);  
    return True;
  return False;

def readArrayParameter(lines, f, name=""):
  n = name;
  if len(name)==0:
    n = str( f.getName() );
  elms = readParameter(lines, n);
  if len(elms)>0:
    values = " ".join(elms);
    print str(f.getName()) + ": " + values;
    f.value = values;
    return True;
  return False;

def arrayParameterToString(f, n=-1, name=""):
  elms = str(f.value).split();
  changedValues = False;
  if len( elms ) < n :
    print "Parameter " + f.getName() + " has less elements than required: left const-padding";
    elms = list(elms[0] for a in range(n-len(elms))) + elms;
    changedValues = True;
  elif len( elms ) > n :
    print "Parameter " + f.getName() + " has more elements than required: left cropping";
    elms = elms[len(elms)-n:];
    changedValues = True;
  values = " ".join(elms);
  if len(name)==0:      
    text = "(" + str( f.getName() ) + " " + values + ")\n";
  else :
    text = "(" + name + " " + values + ")\n";
  
  if changedValues:
    print text;
  return text;

def stringParameterToString(f, name=""):
  if len(name)==0:
    text = "(" + str( f.getName() ) + " \"" + str(f.value) + "\")\n";
  else :
    text = "(" + name + " \"" + str(f.value) + "\")\n";
  return text;

def readBoolParameter(lines, f, name=""):
  n = name;
  if len(name)==0:
    n = str( f.getName() );
  for l in lines:
    if l.find(n) >= 0:
      s = l.replace("(", "");
      s = s.replace(")", "");
      ts = s.split();
      ret = ts[1].replace("\"","");
      print str(f.getName()) + ": " + ret;
      f.setValue( (ret == "true") );
      return True;
  return False;

def boolParameterToString(f, name=""):
  if len(name)==0:
    text = "(" + str( f.getName() ) + " " + bool2str(f.value) + ")\n";
  else :
    text = "(" + name + " " + bool2str(f.value) + ")\n";
  return text;

def readBoolVectorParameter(lines, f, name=""):
  n = name;
  if len(name)==0:
    n = str( f.getName() );
  for l in lines:
    if l.find(n) >= 0:
      s = l.replace("(", "");
      s = s.replace(")", "");
      ts = s.split();
      ret = ts[1].replace("\"","");
      print str(f.getName()) + ": " + ret;
      f.setValue( (ret == "true") );
      return True;
  return False;

def readIntVectorParameter(lines, f, name=""):
  n = name;
  if len(name)==0:
    n = str( f.getName() );
  for l in lines:
    if l.find(n) >= 0:
      s = l.replace("(", "");
      s = s.replace(")", "");
      ts = s.split();
      val = "";
      for v in ts[1:]:
        val += str( int(v) ) + " ";
      print str(f.getName()) + ": " + val;
      f.value = val;
      return True;
  return False;

def boolVectorParameterToString(f, n=-1, name=""):
  if (n<0): 
    n = ctx.field("NumberOfResolutions").value;
  if len(name)==0:
    text = "(" + str( f.getName() ) + " " + strList(bool2str(f.value),n) + ")\n";
  else :
    text = "(" + name + " " + strList(bool2str(f.value),n) + ")\n";  
  return text;

def intVectorParameterToString(f, n=-1, name=""):
  if (n<0): 
    n = ctx.field("NumberOfResolutions").value;
  if len(name)==0:
    text = "(" + str( f.getName() ) + " " + vec2intstr(f.value,n) + ")\n";
  else :
    text = "(" + name + " " + vec2intstr(f.value,n) + ")\n";
  
  return text;

def vectorParameterToString(f, n=-1, name=""):
  if (n<0): 
    n = ctx.field("NumberOfResolutions").value;
  if len(name)==0:
    text = "(" + str( f.getName() ) + " " + vec2str(f.value,n) + ")\n";
  else :
    text = "(" + name + " " + vec2str(f.value,n) + ")\n";
  
  return text;

def readTemplate():
  templateName = ctx.field("templateFile").value;
  
  templateStr = "";
  infile = open(templateName,"r");
  while infile:
    line = infile.readline()
    templateStr += line;
    if len(line) == 0:
      break;
  
  ctx.field("template").value = templateStr;
  infile.close();
  return;

def idList( val, size ):
  L = [val];
  i=1; 
  while i<size:
    L.append(val);
    i+=1;
  return L;

def strList( val, size ):
  L = val;
  i=1; 
  while i<size:
    L += " " + val;
    i+=1;
  return L;

def bool2str( val ):
  if bool(val) :
    return "\"true\"";
  return "\"false\"";

def vec2str( vec, size ):
  s = str(vec[0]);
  i=1; 
  while i<size:
    s += " " + str(vec[i]);
    i+=1;
  return s;

def vec2intstr( vec, size ):
  s = str(int(vec[0]));
  i=1; 
  while i<size:
    s += " " + str(int(vec[i]));
    i+=1;
  return s;
