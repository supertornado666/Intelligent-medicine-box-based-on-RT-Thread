const app = getApp();
const mqtt = require('../../utils/mqtt.min.js')
import Dialog from '@vant/weapp/dialog/dialog';
const aliyunOpt = require('../../utils/aliyun/aliyun_connect.js')
let client = null;  // MQTT服务

function chunkArray(array, size) {
  const result = [];
  for (let i = 0; i < array.length; i += size) {
    result.push(array.slice(i, i + size));
  }
  return result;
}
Page({
  data: {
    title:"RTT 智能药箱小程序",
    isMQTTSeverConnect:false,   //是否连接
    isTopicSubscribe: false,
    isTopicModfiy: false,
    mqttConnectDialog:false,   //是否打开mqtt连接弹窗
    ReminderDialog:false,  //是否打开服药提醒弹窗
    pendingMedicines: [], // 需要服药提醒的药物
    alarmHistory: [], // 用于存储所有报警时间
    lastAlarmTime: null,  //记录最近一次报警时间戳
    currentAlarmStatus: 0, // 默认无报警（0：无，1：有）
    lastAlarmTime: null,
    hasUnreadAlarm: false,
      /*设备与服务逻辑*/
      sensorList: [
        {
            img: "/images/p1.png",
            name: "AHT10",
            parameter: "温度",
            value: 0,
            unit: "°C",
            isPass: false,

        },
        {
            img: "/images/p2.png",
            name: "AHT10",
            parameter: "湿度",
            value: 0,
            unit: "%rh",
            isPass: false,
        },
      ],
      securityList:[
        {
          img:"/images/fingerprint.png",
          name: "指纹认证",
          isOpen: false,
        },
        {
          img:"/images/lock.png",
          name: "药箱门禁",
          isOpen: false,
        },
      ],
      otherList:[
        {
            img:"/images/drug.png",
            title:"药物查询",
            name:"Drug Inquiry",
            idx:0,
        },
        {
            img:"/images/alert.png",
            title:"服药提醒",
            name:"Medication Reminder",
            idx:1,
        },        
        {
            img:"/images/danger.png",
            title:"状况播报",
            name:"Situation Report",
            idx:2,
            status: '正常', // 默认
            statusColor: '#13da2d', // 绿色
        },
        {
            img:"/images/taken_medicine.png",
            title:"服药记录",
            name:"Medication Record",
            idx:3,
        },
        {
            img:"/images/alarm.png",
            title:"报警按钮",
            name:"SOS",
            idx:4,
        },
        {
          img:"/images/introduction.png",
          title:"操作记录",
          name:"Operating Record",
          idx:5,
        },
      ],
      chunkedOtherList: [], // 存储分页数据
      /*药物信息存储*/   
      medicineList: [],  //初始为空       
      //mqtt服务器
      client: null,
      reconnectCounts: 0,

      //MQTT连接的配置
      options: {
          protocolVersion: 3, //MQTT连接协议版本
          clean: false,
          reconnectPeriod: 1000, //1000毫秒，两次重新连接之间的间隔
          connectTimeout: 30 * 1000, //1000毫秒，两次重新连接之间的间隔
          resubscribe: true, //如果连接断开并重新连接，则会再次自动订阅已订阅的主题（默认true）
          clientId: '',
          password: '',
          username: '',
      },
      /*连接输入框*/
        productKey: wx.getStorageSync('productKey') || '',
        deviceName: wx.getStorageSync('deviceName') || '',
        deviceSecret: wx.getStorageSync('deviceSecret') || '',
        regionId: 'cn-shanghai',

        topicGet: wx.getStorageSync('topicGet') || '',
        topicPost: wx.getStorageSync('topicPost') || '',
  },
  
  onLoad: function () {
    const authed = wx.getStorageSync('isAuthed');
    if (!authed) {
      wx.reLaunch({
        url: '/pages/login/login'
      });
      return;  // 防止未登录时继续执行 MQTT 等操作
    }
    const { productKey, deviceName, deviceSecret, topicGet, topicPost } = this.data;
    if (productKey && deviceName && deviceSecret) {
      console.log("检测到设备信息，尝试自动连接 MQTT...");
      this.connectServer();  
      // 自动设置订阅和发布主题
      if (topicGet) {
        wx.setStorageSync('topicGet', topicGet);
      }
      if (topicPost) {
        wx.setStorageSync('topicPost', topicPost);
      }
    } else {
      console.log("未检测到完整 MQTT 配置信息，未自动连接");
    }
    //主要服务页面初始化
    const chunked = this.generateChunkedList(this.data.otherList);
    this.setData({ chunkedOtherList: chunked });
    /*MQTT手动调试*/
    //状态报警功能测试
    // const simulatedPayload = JSON.stringify({
    //   items: {
    //     alarmStatus: { value: 1 ,time: 1749740488889}
    //   },
    // });
    // this.handleMessage('', simulatedPayload);
    // console.log(this.handleMessage);
   },
  
    /*“主要服务”点击逻辑*/
  onOtherSystemTap(e){
    const item = e.currentTarget.dataset.item;
    console.log(item);
       /*打开药物查询页面*/
    if (item.idx == "0") {
      wx.navigateTo({
        url: "/packageA/drug_inquiry/drug_inquiry"
      });
    }
       /*打开服药提醒*/
    if (item.idx == "1") {
      Dialog.confirm({
        title: '确认',
        message: '是否打开服药提醒？',
      }).then(() => {
          this.Remindersystem(); // 用户确认后发送服药提醒
        })
        .catch(() => {
          wx.showToast({
            title: '已取消',
            icon: 'none'
          });
        });
    }
     /*状况播报*/
     if (item.idx === 2) {
      const history = this.data.alarmHistory;
      if (history.length === 0) {
        wx.showModal({
          title: '状况良好',
          content: '一切正常，目前无异常情况~',
          showCancel: false,
          confirmText: '我知道了'
        });
      } else {
        const timeStrList = history.map(ts => this.formatTime(ts)).join('\n');
    
        wx.showModal({
          title: '⚠ 紧急记录',
          content: `以下为服药超时记录：\n${timeStrList}`,
          showCancel: false,
          confirmText: '我知道了',
          success: () => {
            // 如果存在未读报警，清除报警状态
            if (this.data.hasUnreadAlarm) {
              const updatedList = this.data.otherList.map(item => {
                if (item.idx === 2) {
                  return {
                    ...item,
                    status: '正常',
                    statusColor: '#13da2d'
                  };
                }
                return item;
              });
              const chunked = this.generateChunkedList(updatedList);
              this.setData({
                otherList: updatedList,
                chunkedOtherList: chunked,
                currentAlarmStatus: 0,
                hasUnreadAlarm: false
              });
            }
          }
        });
      }
    }
    
     /*打开服药记录页面*/
    if (item.idx == "3") {
      wx.showToast({ title: '功能暂未开放', icon: 'none' });
    }
       /*启动报警按钮*/
    if (item.idx == "4") {
      //this.Alarmsystem();
      Dialog.confirm({
        title: '确认报警',
        message: '是否确认触发报警功能？',
      }).then(() => {
          this.Alarmsystem(); // 用户确认后触发报警
        })
        .catch(() => {
          wx.showToast({
            title: '已取消报警',
            icon: 'none'
          });
        });
    }
       /*打开操作记录页面*/
    if (item.idx == "5") {
      wx.navigateTo({
        url: "/packageA/history/history"
      });
    }
  },

  /*报警开关*/
    Alarmsystem(e) {
      let that = this;
      if (that.data.isMQTTSeverConnect === false) {
          wx.showToast({
              title: '请先连接MQTT服务器',
              icon: 'none',
              duration: 1000
          });
          return;
      }    
      // 发送开启报警的 MQTT 消息
      that.sendAlarmSwitch(1); // 发送开启报警的指令
      wx.showToast({
        title: '报警声鸣响中',
        icon: 'loading',
        duration: 1000
      });    
    },
    // 发布报警指令
    sendAlarmSwitch: function (alarmState) {
        const that = this;
        const timestamp = Date.now().toString();
        const message = {
            AlarmSwitch: alarmState  // 1 或 0
        };
        const topic = `${that.data.productKey}/WX-app-device/user/post`;
        that.data.client.publish(topic, JSON.stringify(message), { qos: 0 }, function (err) {
            if (err) {
              console.error("发布失败:", err);
              wx.showToast({
                title: '发送失败',
                icon: 'none',
                duration: 1500
              });
            } else {
              console.log("发布成功:", message);
              wx.showToast({
                title: alarmState ? '报警已开启' : '报警已关闭',
                icon: 'success',
                duration: 1000
              });
          }
        });
    },

  /*启动服药提醒*/
  Remindersystem() {
    const now = new Date();
    const currentHour = now.getHours();
    const currentMinute = now.getMinutes();
    const currentTimeStr = `${currentHour.toString().padStart(2, '0')}:${currentMinute.toString().padStart(2, '0')}`;
    const nowMinutes = currentHour * 60 + currentMinute;
    const infoList = wx.getStorageSync('medicineInfoList') || [];
    const pending = infoList
      .map(item => {
        const taken = parseInt(item.taken_times) || 0;
        const takeTimes = item.take_time.map(t => {
          const [h, m] = t.split(':').map(Number);
          return { str: t, minutes: h * 60 + m };
        }).sort((a, b) => a.minutes - b.minutes);
        let shouldTakeCount = takeTimes.filter(t => nowMinutes >= t.minutes).length;
        let overdue = taken < shouldTakeCount;
        // 计算 expiredTime：最接近当前时间的，未吃的那一个
        let expiredTime = null;
        for (let i = 0; i < takeTimes.length; i++) {
          if (takeTimes[i].minutes <= nowMinutes && i >= taken) {
            expiredTime = takeTimes[i].str;
            break;
          }
        } 
        // 计算 nextTime
        let nextTime = '无';
        for (let i = 0; i < takeTimes.length; i++) {
          if (takeTimes[i].minutes > nowMinutes && i >= taken) {
            nextTime = takeTimes[i].str;
            break;
          }
        }  
        return {
          ...item,
          overdue,
          expiredTime,
          nextTime,
          shouldTakeCount
        };
      })
      .filter(item => item.taken_times < item.take_time.length); // 今天还没全部吃完的都展示
    this.setData({
      currentTimeStr,
      ReminderDialog: true,
      reminderList: pending
    });
  },
  onReminderClose(){
    this.setData({ReminderDialog:false});
  },
  
  /*打开MQTT连接弹窗*/
  openDialog(){
      this.setData({mqttConnectDialog:true});
  },
  onMqttClose(){
      this.setData({mqttConnectDialog:false});
  },

      // 连接服务器
      connectServer: function () {
        wx.setStorageSync('productKey', this.data.productKey);
        wx.setStorageSync('deviceName', this.data.deviceName);
        wx.setStorageSync('deviceSecret', this.data.deviceSecret);

        var that = this;
        let clientOpt = aliyunOpt.getAliyunIotMqttClient({
            productKey: that.data.productKey,
            deviceName: that.data.deviceName,
            deviceSecret: that.data.deviceSecret,
            regionId: that.data.regionId,
            port: that.data.port,
        });
        let host = 'wxs://' + clientOpt.host;
        this.setData({
            'options.clientId': clientOpt.clientId,
            'options.password': clientOpt.password,
            'options.username': clientOpt.username,
        });
        console.log("this.data.options host:" + host);
        console.log("this.data.options data:" + JSON.stringify(this.data.options));
        this.data.client = mqtt.connect(host, this.data.options);
        getApp().globalData.client = this.data.client;
        this.data.client.on('connect', function (connack) {
          if (that.data.reconnectCounts === 0) {
            wx.showToast({ title: '连接成功' });
          } else {
            wx.showToast({ title: '重新连接成功', icon: 'none', duration: 1000 });
          }        
          that.setData({
            reconnectCounts: 0,
            isMQTTSeverConnect: true
          });        
          // 自动订阅主题
          const topicToSub = wx.getStorageSync('topicGet');
          if (topicToSub) {
            that.subscribeTopic(topicToSub);
          }        
          // 自动设置发布主题地址
          const topicToPub = wx.getStorageSync('topicPost');
          if (topicToPub) {
            that.setData({ topicPost: topicToPub });
          }
        });
        this.data.client.on("message", this.handleMessage.bind(this));
        this.data.client.on("error", function (error) {
            console.log("服务器 error 的回调" + error);
        });
        this.data.client.on("reconnect", function () {
            console.log("服务器 reconnect的回调");
            that.setData({
                reconnectCounts: that.data.reconnectCounts + 1
            });
        });
        this.data.client.on("offline", function (errr) {
            console.log("服务器offline的回调");
            that.setData({
                isMQTTSeverConnect: false
            });
        });
    },
    // 订阅主题
    subscribeTopic: function (topic) {
      var that = this;
      if (this.data.client && this.data.client.connected) {
          this.data.client.subscribe(topic, function (err, granted) {
              if (!err) {
                  wx.showToast({
                      title: '订阅主题成功'
                  });
                  wx.setStorageSync('subscribedTopic', topic);
                  that.setData({
                      isTopicSubscribe: true
                  });
              } else {
                  that.setData({
                      isTopicSubscribe: false,
                  });
                  wx.showToast({
                      title: '订阅主题失败',
                      icon: 'fail',
                      duration: 2000
                  });
              }
          });
      } else {
          wx.showToast({
              title: '请先连接服务器',
              icon: 'none',
              duration: 2000
          });
      }
  },

  // 取消订阅主题
  unsubscribeTopic: function (topic) {
      if (this.data.client && this.data.client.connected) {
          this.data.client.unsubscribe(topic);
          this.setData({
              isTopicSubscribe: false
          });
          wx.showToast({
              title: '取消订阅成功'
          });
      } else {
          wx.showToast({
              title: '请先连接服务器',
              icon: 'none',
              duration: 2000
          });
      }
  },

  // 发布消息
  publishTopic: function (topic, message) {
      let transdata={cmd:message}
          
      console.log(JSON.stringify(transdata));
      if (this.data.client && this.data.client.connected) {
          this.data.client.publish(topic, JSON.stringify(transdata));
          wx.showToast({
              title: '发布成功'
          });
      } else {
          wx.showToast({
              title: '请先连接服务器',
              icon: 'none',
              duration: 800
          });
      }
  },

  // 处理接收到的消息
  handleMessage: function (topic, payload) { 
    //console.log("收到 topic:" + topic + " , payload :" + payload);
    let getMessageObj = {};
    try {
      getMessageObj = JSON.parse(payload);
    } catch (e) {
      console.error("JSON 解析失败", e);
      return;
    }
    console.log("items:",getMessageObj.items);
    const items = getMessageObj.items || {};

    // ------- 处理温湿度信息 -------
    if (items.CurrentTemperature || items.CurrentHumidity) {
      this.setData({
        sensorList: this.data.sensorList.map(sensor => {
          let updatedSensor = { ...sensor };
          if (sensor.parameter === "温度" && items.CurrentTemperature && typeof items.CurrentTemperature.value === "number") {
            updatedSensor.value = items.CurrentTemperature.value;
            updatedSensor.isPass = true;
          }
          if (sensor.parameter === "湿度" && items.CurrentHumidity && typeof items.CurrentHumidity.value === "number") {
            updatedSensor.value = items.CurrentHumidity.value;
            updatedSensor.isPass = true;
          }
          return updatedSensor;
        })
      });
    }
     // ------- 处理指纹认证和门禁 -------
      if (items.fingerPrintValue || items.doorIsOpen) {
        const newList = [...this.data.securityList];

        if (items.fingerPrintValue) {
          newList[0].isOpen = items.fingerPrintValue.value === 1;
          this.logOperation(`指纹认证成功`);
          wx.showToast({
            title: `指纹认证成功`,
            icon: 'success',
            duration: 1000
          });
        }
        if (items.doorIsOpen) {
          newList[1].isOpen = items.doorIsOpen.value === 1;
          this.logOperation(`药箱门禁打开`);
        }
        this.setData({ securityList: newList });
      }

      // ------- 处理药物信息 -------
      if (items.medicineInfo && items.medicineInfo.value) {
        const raw = items.medicineInfo.value;

        // 预处理 drugTaketime（清除所有非数字、非冒号、非空格字符）
        const cleanTaketime = raw.drugTaketime.replace(/[^0-9:\s]/g, '').trim();
        const takeTimes = raw.drugTaketime.match(/\d{1,2}:\d{2}/g) || [];

        // 检查关键字段
        if (raw.drugName && takeTimes.length > 0 && raw.singleAmount != null && raw.takenTime != null && raw.Location != null) {
          const formattedMedicine = {
            name: raw.drugName,
            take_time: takeTimes,
            amount: raw.singleAmount,
            taken_times: Number(raw.takenTime),
            number: raw.Location
          };

          let existingList = wx.getStorageSync('medicineInfoList') || [];
          const index = existingList.findIndex(item => item.name === formattedMedicine.name);

          if (index !== -1) {
            const existing = existingList[index];
            const isIdentical =
              JSON.stringify(existing.take_time) === JSON.stringify(formattedMedicine.take_time) &&
              existing.amount === formattedMedicine.amount &&
              existing.taken_times === formattedMedicine.taken_times &&
              existing.number === formattedMedicine.number;

            if (!isIdentical) {
              existingList[index] = formattedMedicine;
              wx.setStorageSync('medicineInfoList', existingList);
              wx.showToast({
                title: `已更新药物：${formattedMedicine.name}`,
                icon: 'success',
                duration: 2000
              });
            } else {
              console.log(`收到重复药物信息：${formattedMedicine.name}，未作处理`);
            }
          } else {
            existingList.push(formattedMedicine);
            wx.setStorageSync('medicineInfoList', existingList);
            wx.showToast({
              title: `已添加药物：${formattedMedicine.name}`,
              icon: 'success',
              duration: 2000
            });
          }
        } else {
          console.warn("药物信息字段不完整，未保存：", raw);
        }
      }
      // ------- 处理报警状态 -------
      if (items.alarmStatus && typeof items.alarmStatus.value === "number") {
        const alarmValue = items.alarmStatus.value;
        const time = items.alarmStatus.time || Date.now();
        this.logOperation(`超过服药时间,触发警报`);
        if (alarmValue === 1) {
          const timeStr = this.formatTime(time);
      
          // 弹窗提示
          wx.showModal({
            title: '⚠ 紧急状况',
            content: `老人在 ${timeStr} 超过服药时间，请立即查看！`,
            showCancel: false,
            confirmText: '我知道了'
          });
      
          // 添加到历史记录
          const newHistory = [...this.data.alarmHistory, time];
          // 更新显示
          const updatedList = this.data.otherList.map(item => {
            if (item.idx === 2) {
              return {
                ...item,
                status: '有紧急情况',
                statusColor: '#ff0000'
              };
            }
            return item;
          });
      
          const chunked = this.generateChunkedList(updatedList);
          this.setData({
            otherList: updatedList,
            chunkedOtherList: chunked,
            currentAlarmStatus: 1,
            lastAlarmTime: time,
            alarmHistory: newHistory,
            hasUnreadAlarm: true
          });
        }
      }
  },

    // 断开服务器
    disconnectServer: function () {
        if (this.data.client && this.data.client.connected) {
            this.data.client.end();
            this.setData({
                isMQTTSeverConnect: false,
                isTopicSubscribe: false,
                isTopicModify: false,
            });
            wx.showToast({
                title: '断开连接'
            });
        } else {
            wx.showToast({
                title: '服务器未连接',
                icon: 'none',
                duration: 2000
            });
        }
    },
      // 连接服务器按钮点击事件
      onClick_connect: function () {
        this.connectServer();
    },
    // 断开服务器按钮点击事件
    onClick_disconnect: function () {
        this.disconnectServer();
    },
    // 订阅按钮点击事件
    onClick_SubOne: function () {
        if (this.data.isMQTTSeverConnect === false) {
            wx.showToast({
                title: '请先连接MQTT服务器',
                icon: 'none',
                duration: 1000
            });
            return;
        }
        wx.setStorageSync('topicGet', this.data.topicGet);
        this.subscribeTopic(this.data.topicGet);
    },
    // 取消订阅按钮点击事件
    onClick_unSubOne: function () {
        this.unsubscribeTopic(this.data.topicGet);
    },
    // 发布主题按钮确认点击事件
    onClick_confirmPost: function () {
        var that = this;
        if (that.data.isMQTTSeverConnect === false) {
            wx.showToast({
                title: '请先连接MQTT服务器',
                icon: 'none',
                duration: 1000
            });
            return;
        }

        wx.setStorageSync('topicPost', that.data.topicPost);
        wx.showToast({
            title: '修改成功'
        });
        that.setData({
            isTopicModify: true
        });
        // this.publishTopic(that.data.topicGet, 'test i am from wechat msg');
        // console.log("isTopicModify:",that.data.isTopicModify);
    },
    // 修改主题按钮点击事件
    onClick_modifyPost: function () {
        var that = this;
        wx.showToast({
            title: '修改发布主题',
            icon: 'none',
            duration: 2000,
        });
        that.setData({
            isTopicModify: false
        });
        // console.log("isTopicModify:",that.data.isTopicModify);
    },

    /*工具函数*/
    // 分页函数
    generateChunkedList(list) {
        const chunked = [];
        // 前4个功能块：组成 2×2 的第一页
        const firstPage = list.slice(0, 4);
        chunked.push(firstPage);
        // 剩余功能块：组成 2×1 的第二页
        const secondPage = list.slice(4);
        if (secondPage.length) chunked.push(secondPage);
        return chunked;
    },
    /*时间格式化函数*/
    formatTime(ts) {
      if (!ts) return '未知时间';
      const date = new Date(ts);
      const hour = date.getHours().toString().padStart(2, '0');
      const minute = date.getMinutes().toString().padStart(2, '0');
      return `${hour}:${minute}`;
    },
    /*操作记录函数*/
    logOperation(text) {
      const logs = wx.getStorageSync('operationLogs') || [];
      const time = new Date().toLocaleString(); // 格式: 2025/6/10 上午10:45:01
      logs.push({ text, time });
      wx.setStorageSync('operationLogs', logs);
    }
})
