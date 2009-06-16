#include "../AnimationExecuter.h"

ML_START_NAMESPACE


void AnimationExecuter::commandFading(vector<string>* tempEntryParams, kScriptFrameEntry* currentEntry, int frameNr, size_t FrameEntryListIter)
{
	if (tempEntryParams->size()>0)
	{
		bool fading = ((string)(*tempEntryParams)[0]) == "TRUE";
		kDebug::Debug("execute fading command",kDebug::DL_LOW);
		string data = "";
		waitingFor_execFading = true;
		gotFading = false;
		returnFrame = frameNr;
		returnEntry = FrameEntryListIter+1;

		if (fading)
		{
			data = currentEntry->getObjectID();
			data += " TRUE";
		}
		else
		{
			data = "NONE FALSE";
		}

		kDebug::Debug("send Fading request data="+data,kDebug::DL_HIGH);

		_modMsgSenderExecFading->getFieldContainer()->getField("data")->setStringValue(data);
		_modMsgSenderExecFading->getFieldContainer()->getField("message")->setStringValue("fading");
		((NotifyField*)_modMsgSenderExecFading->getFieldContainer()->getField("send"))->notify();						
	}
}

ML_END_NAMESPACE