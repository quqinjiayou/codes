#include <json/json.h>

class DialogInput {
public:
	string type;
	string str;		// original input string
	map<string, string> mapPropertyValue;
	Json::Value inputJson;
public:
	string GetUttid() {
		map<string,string>::iterator it = mapPropertyValue.find("uttid");
		return mapPropertyValue.end() != it ? (it->second) : "";
	}

	string GetInput() {
		map<string,string>::iterator it = mapPropertyValue.find("input");
		return mapPropertyValue.end() != it ? (it->second) : "";
	}

	string GetType() {
		return type;
	}


	bool FromString(string strFrame) {
	//TODO: fill this function for 
		str = strFrame;
		//TODO: use json to endode and decode the properties;

		return false;

	}
};

/**
 * @Title �Ի���ͼ
 * @Brief targets��ʾĿ�����ݣ�paramValueMap��ʾ��ѯ����
 */
class DialogIntent {

private:
	/** true��ʾ�������������߼�ֵΪTrue, ���Ի�������ֹ�� */
    bool operable;

    /**
     * Ŀ���ѯ��������
     * �ڵ�һ���������У�targets��Ĭ�ϡ�Ψһ�ģ�����Ҫָ����
     * �ڶ����������У�targets��Ҫָ��������TV�в���ӽ�Ŀ�������̨���ֲ�ͬ������
     */
    list<string> targets;

    /** ��ѯ����������ѯ�������Ͳ���ֵ */
    //	private List<Pair<String, String>> params;
    map<string, string> paramValueMap;

public:
	DialogIntent() : operable(false) {}
    void SetStatus(bool st) {
        operable = st;
    }

    /** ����ֵ��ʾ��ǰ�Ƿ������ִ�еĶԻ�״̬ */
    bool IsActionable() {
        return operable;
    }

    /** getĿ�����ݣ�����ѯĿ���������� */
    list<string> GetTargetList() {
        return targets;
    }

    /** get��ѯ����������ѯ�������Ͳ���ֵ */
    map<string, string> GetParamValueMap() {
        return paramValueMap;
    }

    /** setĿ�����ݣ�����ѯĿ���������� */
    void SetTargetList(list<string> targets) {
        this->targets = targets;
    }

    /** get��ѯ����������ѯ�������Ͳ���ֵ */
    void SetParamValueMap(map<string, string> pvMap) {
        this->paramValueMap = pvMap;
    }

    string ToString() {
		map<string,string> propertyMap = GetParamValueMap();
		string strPropertys;
		for (map<string, string>::iterator it = propertyMap.begin(); propertyMap.end() != it; ++it) {
			strPropertys += (it->first + ":" + it->second + "\n");
		}
        if (IsActionable()) {
            return "Actionable: " + strPropertys;
        } else {
            return "NOT Actionable: " + strPropertys;

        }
    }


};

class ExecuteResult {
 public:
 	TDialogExecuteReturnCode returnCode;
	int outputType; // 1: string, plain text; 2: json; 3: none;
	string output;

public:
TDialogExecuteReturnCode GetReturnCode() {
	return returnCode;
	}
};

class DialogResponse {
    
public:
	static string DIALOG_QUIT_GREETING;
    
private:
	string replyMessage;
	string code;
	string replyData;
	DialogIntent intent;
	
public:

	void SetReturnCode(string code) {

	}

	void SetData(string data) {
		replyData = data;
	}

	bool IsActionable() {
		return intent.IsActionable();
	}
	
	string GetReplyMessage() {
		return replyMessage;
	}
	
	void SetReplyMessage(string msg) {
		this->replyMessage = msg;
	}
	
	/** ��ȡ�Ի���ͼ
	 *	null ��ʾ��ǰû���㹻����Ϣִ�������Ĳ���
	 */
	DialogIntent GetIntent() {
		return intent;
	}
	
	void SetIntent(DialogIntent intent) {
		this->intent = intent;
	}

	
	string ToString() {
	    if (IsActionable()) {
	        return "Actionable:\n" + replyMessage + "\n" + GetIntent().ToString();
	    } else {
	        return replyMessage;
	    }
	}
	
	

};

