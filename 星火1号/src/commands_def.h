/************HMI-Board*************/

#define IDENTITY_BIND 			"1"//身份绑定
#define IDENTITY_UNBIND 		"2"//解除绑定
#define MEDICINE_IN_BEGIN 		"3"//录入药物开始，等待扫描成功
#define MEDICINE_IN_END 		"4"//录入药物结束
#define MEDICINE_OUT_BEGIN 		"5"//移除药物开始，按下按钮发送对应移除格子
#define MEDICINE_OUT_END 		"6"//移除药物结束

#define MEDICINE_OUT_1 			"7"//移除第一格药物
#define MEDICINE_OUT_2			"8"//移除第二格药物
#define MEDICINE_OUT_3 			"9"//移除第三格药物
#define MEDICINE_OUT_4 			"A"//移除第四格药物
#define MEDICINE_OUT_5 			"B"//移除第五格药物

#define GET_TIME				"L"//获取时间，显示在面板上（定时获取，如1小时）
#define GET_TEMP_HUMI			"O"//获取温湿度，显示在面板上（定时获取，如1分钟）

//语音交互内容
#define MEDICINE_GET_INFO		"C"//获取当前药物信息，以发送至DeepSeek分析
#define MEDICINE_TIME_GET		"U"//获取下一次服药时间
//......

/************HMI-Board*************/

/*************RT-Spark*************/

#define IDENTITY_SUCCESS		"D"//身份 绑定/解绑 成功
#define MEDICINE_INFOIN_SUCCESS	"E"//条码扫描成功，随后等待面板发送服用频次

#define SHOW_SMILE				"F"//显示笑脸
#define SHOW_SAD				"G"//显示苦脸
#define SHOW_QUESTION			"H"//显示疑问
#define SHOW_EMO_END			"I"//显示结束

#define MEDICINE_OUT_SUCCESS	"M"//移除药物成功
#define MEDICINE_OUT_ERROR		"N"//药物格无药物或其他错误

#define MEDICINE_TIME_ON		"P"//用药时间到，然后发送需要服用的药物的频次
#define IDENTITY_WRONG			"R"//身份不对
#define MEDICINE_TAKE_WRONG		"Q"//取药不对
#define NOT_LOCKED				"T"//未锁好箱子
#define MEDICINE_TIME_TIMEOUT	"S"//未按时服药

//To OpenMV
#define START_SCAN				"J"//开始扫描条码，每扫到一条需重新发送
#define FINISH_SCAN				"K"//结束扫描

/*************RT-Spark*************/
