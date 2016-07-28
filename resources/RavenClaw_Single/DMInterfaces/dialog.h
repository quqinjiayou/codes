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
 * @Title 对话意图
 * @Brief targets表示目标数据，paramValueMap表示查询参数
 */
class DialogIntent {

private:
	/** true表示主属性子树的逻辑值为True, 即对话可以终止了 */
    bool operable;

    /**
     * 目标查询数据名称
     * 在单一任务领域中，targets是默认、唯一的，不需要指定；
     * 在多任务领域中，targets需要指定，比如TV中查电视节目、查电视台两种不同的任务；
     */
    list<string> targets;

    /** 查询参数，即查询参数名和参数值 */
    //	private List<Pair<String, String>> params;
    map<string, string> paramValueMap;

public:
	DialogIntent() : operable(false) {}
    void SetStatus(bool st) {
        operable = st;
    }

    /** 返回值表示当前是否满足可执行的对话状态 */
    bool IsActionable() {
        return operable;
    }

    /** get目标数据，即查询目标数据名称 */
    list<string> GetTargetList() {
        return targets;
    }

    /** get查询参数，即查询参数名和参数值 */
    map<string, string> GetParamValueMap() {
        return paramValueMap;
    }

    /** set目标数据，即查询目标数据名称 */
    void SetTargetList(list<string> targets) {
        this->targets = targets;
    }

    /** get查询参数，即查询参数名和参数值 */
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
	
	/** 获取对话意图
	 *	null 表示当前没有足够的信息执行期望的操作
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

