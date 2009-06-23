#include "../AnimationExecuter.h"

ML_START_NAMESPACE

void AnimationExecuter::commandVolRend(/*vector<string>* tempEntryParams,*/ kScriptFrameEntry* currentEntry, int frameNr/*, size_t FrameEntryListIter*/)
{
	//Im ersten Frame einer VolRendAlpha-Anweisung werden alle weiteren Transferfunktionen für die übrigen Frames des Anweisung berechnet
	//Dabei wird für alle weiteren Frames der Anweisung auch das CFLAG_EXECUTEVOLREND gesetzt					
	//std::cout << "VolRend" << std::endl;
	myObjMgr->setObjAttribute("METKVolViz",L_GLOBAL,LAY_GLOBAL_REFRESH,new string("1"),omINFOTYPE_MESSAGE,true,false);
	sendNotification();

	if (currentEntry->getCommunicationFlag()!=kScriptFrameEntry::CFLAG_EXECUTEVOLREND)
	{
		kDebug::Debug("calculate transferfunctions for volrend command",kDebug::DL_LOW);
		preCalcTransferFunctions(currentEntry,frameNr);
	}					
	writeCalculatedTransferFunctions(currentEntry);
}


void AnimationExecuter::writeCalculatedTransferFunctions(kScriptFrameEntry* currentEntry)
{	
	ML_TRACE_IN("AnimationExecuter::writeCalculatedTransferFunctions()");
	std::cout << "writeCalculatedTransferFunctions     paramStr = " << currentEntry->getParamString() <<  std::endl;
	if (currentEntry->getCommunicationFlag()==kScriptFrameEntry::CFLAG_EXECUTEVOLREND)
	{					
		string pureParamStr = currentEntry->getParamString();
		vector<string> splittedParam;
		kBasics::split(pureParamStr,';',99,&splittedParam);		

		if (splittedParam.size()==2)
		{						
			myObjMgr->setObjAttribute("METKVolViz",LAY_TRANSFERFUNCTION,INF_ALPHAPOINTS,&(splittedParam[0]),omINFOTYPE_MESSAGE,true,true);			
			myObjMgr->setObjAttribute("METKVolViz",LAY_TRANSFERFUNCTION,INF_COLORPOINTS,&(splittedParam[1]),omINFOTYPE_MESSAGE,true,true);			
		}
	}
}


void AnimationExecuter::preCalcTransferFunctions(kScriptFrameEntry* frameEntry, int globalFrameNr)
{
	ML_TRACE_IN("AnimationExecuter::preCalcTransferFunctions()");
	std::cout << "preCalcTransferFunctions" << std::endl;

	string pureParamStr = frameEntry->getParamString();
	vector<string> splittedParam;
	kBasics::split(pureParamStr,';',99,&splittedParam);

	if (splittedParam.size()!=2)
	{
		std::cout << "Wrong parameter format for VolRend. Use [AlphaValues];[ColorValues]" << std::endl;
		return;
	}

	string sourceAlphaString; myObjMgr->getObjAttributeString("METKVolViz",LAY_TRANSFERFUNCTION,INF_ALPHAPOINTS,sourceAlphaString);
	string sourceColorString; myObjMgr->getObjAttributeString("METKVolViz",LAY_TRANSFERFUNCTION,INF_COLORPOINTS,sourceColorString);
	string targetAlphaString = splittedParam[0];
	string targetColorString = splittedParam[1];

	/*std::cout << "sourceAlphaString " << sourceAlphaString << std::endl;
	std::cout << "sourceColorString " << sourceColorString << std::endl;
	std::cout << "targetAlphaString " << targetAlphaString << std::endl;
	std::cout << "targetColorString " << targetColorString << std::endl;*/

	//Splitten in Listen
	vector<TFPoint> sourceList;
	vector<TFPoint> sourceListExtended;
	vector<TFPoint> targetList;
	vector<TFPoint> targetListExtended;

	sourceAlphaString = kBasics::replaceString(sourceAlphaString,"["," ");	sourceAlphaString = kBasics::replaceString(sourceAlphaString,"]"," "); sourceAlphaString = kBasics::trimString(sourceAlphaString);
	sourceColorString = kBasics::replaceString(sourceColorString,"["," ");	sourceColorString = kBasics::replaceString(sourceColorString,"]"," "); sourceColorString = kBasics::trimString(sourceColorString);
	targetAlphaString = kBasics::replaceString(targetAlphaString,"["," ");	targetAlphaString = kBasics::replaceString(targetAlphaString,"]"," "); targetAlphaString = kBasics::trimString(targetAlphaString);
	targetColorString = kBasics::replaceString(targetColorString,"["," ");	targetColorString = kBasics::replaceString(targetColorString,"]"," "); targetColorString = kBasics::trimString(targetColorString);

	//Creating the source list from Alpha and Color strings from the ObjMgr
	//diese lustigen Klammern sind nur für die Übersicht zum Ausblenden
	{
		vector<string> splittedPointsAlpha; vector<string> splittedPointsColor;
		vector<string> splittedPairAlpha; vector<string> splittedPairColor;

		kBasics::split(sourceAlphaString,',',255,&splittedPointsAlpha);
		kBasics::split(sourceColorString,',',255,&splittedPointsColor);

		if (splittedPointsAlpha.size()!=splittedPointsColor.size())
		{
			std::cout << "Unequal number of points in SOURCE transferfunction for color and alpha values" << std::endl;
			return;
		}

		for (vector<string>::size_type i=0; i<splittedPointsAlpha.size(); i++)
		{
			kBasics::split(kBasics::trimString(splittedPointsAlpha[i]),' ',2,&splittedPairAlpha);
			kBasics::split(kBasics::trimString(splittedPointsColor[i]),' ',4,&splittedPairColor);
			if (splittedPairAlpha.size()!=2 || splittedPairColor.size()!=4)
			{
				std::cout << "Wrong color or opacity values in SOURCE transferfunction" << std::endl;
				std::cout << "Opacity split size:" << splittedPairAlpha.size() << std::endl;
				std::cout << "Color split size:" << splittedPairColor.size() << std::endl;
				return;
			}
			TFPoint tempPoint;
			try
			{
				tempPoint.voxel = kBasics::StringToFloat(splittedPairAlpha[0]);
				tempPoint.opacity = kBasics::StringToFloat(splittedPairAlpha[1]);
				tempPoint.color[0] = kBasics::StringToFloat(splittedPairColor[1]); tempPoint.color[1] = kBasics::StringToFloat(splittedPairColor[2]); tempPoint.color[2] = kBasics::StringToFloat(splittedPairColor[3]);
				sourceList.push_back(tempPoint);
			}
			catch (...)
			{
				std::cout << "invalid char in SOURCE transferfunction" << std::endl;
			}
		}
	}

	//Creating the target list from Alpha and Color strings from the script command
	//diese lustigen Klammern sind nur für die Übersicht zum Ausblenden
	{
		vector<string> splittedPointsAlpha; vector<string> splittedPointsColor;
		vector<string> splittedPairAlpha; vector<string> splittedPairColor;


		kBasics::split(targetAlphaString,',',255,&splittedPointsAlpha);
		kBasics::split(targetColorString,',',255,&splittedPointsColor);

		if (splittedPointsAlpha.size()!=splittedPointsColor.size())
		{
			std::cout << "Unequal number of points in TARGET transferfunction for color and alpha values" << std::endl;
			return;
		}

		for (vector<string>::size_type i=0; i<splittedPointsAlpha.size(); i++)
		{
			kBasics::split(kBasics::trimString(splittedPointsAlpha[i]),' ',2,&splittedPairAlpha);
			kBasics::split(kBasics::trimString(splittedPointsColor[i]),' ',4,&splittedPairColor);
			if (splittedPairAlpha.size()!=2 || splittedPairColor.size()!=4)
			{
				std::cout << "Wrong color or opacity values in TARGET transferfunction" << std::endl;
				return;
			}

			TFPoint tempPoint;
			try
			{
				tempPoint.voxel = kBasics::StringToFloat(splittedPairAlpha[0]);
				tempPoint.opacity = kBasics::StringToFloat(splittedPairAlpha[1]);
				tempPoint.color[0] = kBasics::StringToFloat(splittedPairColor[1]); tempPoint.color[1] = kBasics::StringToFloat(splittedPairColor[2]); tempPoint.color[2] = kBasics::StringToFloat(splittedPairColor[3]);
				targetList.push_back(tempPoint);
			}
			catch (...)
			{
				std::cout << "invalid char in TARGET transferfunction" << std::endl;
			}
		}
	}


	//Auffüllen der jeweiligen Listen, damit zwischen Listen gleicher Punktanzahl und Voxelwerte interpoliert werden kann
	vector<TFPoint>::size_type countTarget=0;
	vector<TFPoint>::size_type countSource=0;
	TFPoint tempPoint;
	tempPoint.opacity=0; tempPoint.color=vec3(0,0,0);
	TFPoint value1, value2;

	while (countTarget<targetList.size() || countSource<sourceList.size())
	{
		if (countTarget<targetList.size())
		{
			if (countSource<sourceList.size())
			{
				if (targetList[countTarget].voxel==sourceList[countSource].voxel)
				{
					tempPoint.voxel = sourceList[countSource].voxel;
					tempPoint.opacity = sourceList[countSource].opacity;
					tempPoint.color = sourceList[countSource].color;
					sourceListExtended.push_back(tempPoint);
					countSource++;

					tempPoint.opacity = targetList[countTarget].opacity;
					tempPoint.color = targetList[countTarget].color;
					targetListExtended.push_back(tempPoint);					
					countTarget++;
				}
				else if (targetList[countTarget].voxel<sourceList[countSource].voxel)
				{
					tempPoint.voxel = targetList[countTarget].voxel;

					//Opacity&Color in source interpolieren
					if (countSource>0) value1=sourceList[countSource-1];		
					else
					{
						value1=sourceList[countSource];
						value1.voxel=targetList[countTarget].voxel;
					}
					value2=sourceList[countSource];
					tempPoint.opacity = value1.opacity + (value2.opacity-value1.opacity)*(tempPoint.voxel-value1.voxel)/(value2.voxel-value1.voxel);					
					tempPoint.color = value1.color + (value2.color-value1.color)*double((tempPoint.voxel-value1.voxel)/(value2.voxel-value1.voxel));
					sourceListExtended.push_back(tempPoint);

					tempPoint.opacity = targetList[countTarget].opacity;
					tempPoint.color = targetList[countTarget].color;
					targetListExtended.push_back(tempPoint);
					countTarget++;
				}
				else //if (targetList[countTarget].voxel>sourceList[countSource].voxel)
				{
					//std::cout << "source: " << sourceList[countSource].voxel << std::endl;
					tempPoint.voxel = sourceList[countSource].voxel;
					tempPoint.opacity = sourceList[countSource].opacity;
					tempPoint.color = sourceList[countSource].color;
					sourceListExtended.push_back(tempPoint);

					//Opacity&Color in target interpolieren
					if (countTarget>0) value1=targetList[countTarget-1];		
					else
					{
						value1=targetList[countTarget];
						value1.voxel=sourceList[countSource].voxel;
					}
					value2=targetList[countTarget];
					tempPoint.opacity = value1.opacity + (value2.opacity-value1.opacity)*(tempPoint.voxel-value1.voxel)/(value2.voxel-value1.voxel);
					tempPoint.color = value1.color + (value2.color-value1.color)*double((tempPoint.voxel-value1.voxel)/(value2.voxel-value1.voxel));
					targetListExtended.push_back(tempPoint);
					countSource++;
				} 
			} 
			else
			{
				tempPoint.voxel = targetList[countTarget].voxel;
				tempPoint.opacity = sourceList[countSource-1].opacity; //countSource-1, da es schon um +1 über size hinaus ist
				tempPoint.color = sourceList[countSource-1].color; //countSource-1, da es schon um +1 über size hinaus ist
				sourceListExtended.push_back(tempPoint);
				tempPoint.opacity = targetList[countTarget].opacity;
				tempPoint.color = targetList[countTarget].color;
				targetListExtended.push_back(tempPoint);
				countTarget++;
			}			
		}
		else
		{
			tempPoint.voxel = sourceList[countSource].voxel;
			tempPoint.opacity = sourceList[countSource].opacity;
			tempPoint.color = sourceList[countSource].color;
			sourceListExtended.push_back(tempPoint);
			tempPoint.opacity = targetList[countTarget-1].opacity; //countTarget-1, da es schon um +1 uber size hinaus ist
			tempPoint.color = targetList[countTarget-1].color; //countTarget-1, da es schon um +1 uber size hinaus ist
			targetListExtended.push_back(tempPoint);
			countSource++;
		}
	}


	vector<TFPoint> lastValue;
	for (vector<TFPoint>::size_type k=0; k<sourceListExtended.size(); k++)
	{
		lastValue.push_back(sourceListExtended[k]);
	}

	float newOpacity;
	vec3 newColor;
	for (int i=0; i<=frameEntry->getFrameCount(); i++)
	{	
		std::cout << "i=" << i << "   frameCount:" << frameEntry->getFrameCount() << std::endl;
		//Write values as parameter in corresponding FrameEntry

		//kDebug::Debug("globalFrameNr:"+kBasics::IntToString(globalFrameNr), kDebug::DL_HIGH);
		//kDebug::Debug("i:"+kBasics::IntToString(i), kDebug::DL_HIGH);
		//kDebug::Debug("frameEntry->getCommandLine()"+frameEntry->getCommandLine(),kDebug::DL_HIGH);
		int j = myList.FrameList[globalFrameNr+i]->getEntryIDByCommandLine(frameEntry->getCommandLine());
		//kDebug::Debug("j:"+kBasics::IntToString(j), kDebug::DL_HIGH);
		if (j!=-1)
		{	
			string ParamStrAlpha, ParamStrColor;

			if (i<frameEntry->getFrameCount())
			{
				ParamStrAlpha = "["; ParamStrColor = "[";

				for (vector<TFPoint>::size_type i2=0; i2<targetListExtended.size();i2++)
				{
					newOpacity = TFPoint(lastValue[i2]).opacity + ( targetListExtended[i2].opacity - TFPoint(lastValue[i2]).opacity ) / ( frameEntry->getFrameCount() - i + 1.0 );
					newColor = TFPoint(lastValue[i2]).color + ( targetListExtended[i2].color - TFPoint(lastValue[i2]).color ) / double ( frameEntry->getFrameCount() - i + 1.0 );
					ParamStrAlpha+= kBasics::DoubleToString(targetListExtended[i2].voxel)+" "+kBasics::DoubleToString(newOpacity);
					ParamStrColor+= kBasics::DoubleToString(targetListExtended[i2].voxel)+" "
						+kBasics::DoubleToString(newColor[0])+" "
						+kBasics::DoubleToString(newColor[1])+" "
						+kBasics::DoubleToString(newColor[2]);

					if (i2<targetListExtended.size()-1) //noch mind. ein Eintrag folgt
					{
						ParamStrAlpha+=", "; 
						ParamStrColor+=", ";
					}

					lastValue[i2].opacity = newOpacity;
					lastValue[i2].color = newColor;
				}

				ParamStrAlpha+= "]"; ParamStrColor+= "]";
			}
			else //Last frame set original target values
			{				
				ParamStrAlpha = splittedParam[0];
				ParamStrColor = splittedParam[1];
				std::cout << "xxxxxxxxxxxxxx Param = " << ParamStrAlpha << ";" << ParamStrColor << std::endl;
			}

			myList.FrameList[globalFrameNr+i]->EntryList[j]->setParamString(ParamStrAlpha+";"+ParamStrColor);
			myList.FrameList[globalFrameNr+i]->EntryList[j]->setCommunicationFlag(kScriptFrameEntry::CFLAG_EXECUTEVOLREND);

			kDebug::Debug("setParamString for frame:"+kBasics::IntToString(globalFrameNr+i)+"  Entry:"+kBasics::IntToString(j)+"  ParamStr: *"+ParamStrAlpha+";"+ParamStrColor+"*",kDebug::DL_HIGH);			
		}
		else kDebug::Debug("Frame entry, that should be there, is it not :-(",kDebug::DL_HIGH);		
	}
}

ML_END_NAMESPACE