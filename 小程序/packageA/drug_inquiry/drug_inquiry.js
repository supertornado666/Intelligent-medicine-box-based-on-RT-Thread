Page({
  data: {
    search_value: '', // 绑定输入框内容
    medicineList: [],// 原始数据列表   
    medicineSearch: [],  // 搜索结果列表 
    showEditDialog: false,   
    editData: {},
    editIndex: null,
    isAddMode: false, // 是否为添加模式
  },
  
  onLoad: function () {
    const infoList = wx.getStorageSync('medicineInfoList') || [];
    console.log("当前本地缓存中的药物信息：", infoList);
    this.sortMedicineList(infoList);
    this.setData({
      medicineList: infoList,
      medicineSearch: infoList,
      client: getApp().globalData.client,
      productKey: wx.getStorageSync('productKey')
      }, () => {
      //this.requestMedicineInfo();
    });
  },

// 主动请求药物信息
requestMedicineInfo() {
  const that = this;
  if (!that.data.client || !that.data.client.connected) {
    wx.showToast({
      title: '请先连接MQTT服务器',
      icon: 'none',
      duration: 1500
    });
    return;
  }
  const message = {
    cmd: "RequestMedicineInfo"
  };
  const topic = `${that.data.productKey}/WX-app-device/user/post`;

  that.data.client.publish(topic, JSON.stringify(message), { qos: 0 }, function (err) {
    if (err) {
      console.error("请求药物信息失败:", err);
      wx.showToast({
        title: '请求失败',
        icon: 'none',
        duration: 1000,
      });
    } else {
      console.log("向板端请求药物信息成功:", message);
      wx.showToast({
        title: '加载中...',
        icon: 'loading',
        duration: 500,
      });
    }
  });
  console.log("client对象：", that.data.client);
  console.log("client连接状态：", that.data.client && that.data.client.connected);
},
  // 根据 number 排序药物信息列表
  sortMedicineList(medicineList) {
    medicineList.sort((a, b) => a.number - b.number);  // 升序排序
  },
  
  /*页面返回函数*/
  goBack() {
    wx.navigateBack({
      delta: 1
    });
  },

  // 用户点击“搜索”按钮
  onSearch(event) {
    const keyword = event.detail; // 用户输入内容
    const result = this.data.medicineList.filter(item =>
      item.name.includes(keyword.trim())
    );
    
    // 更新排序后的搜索结果
    this.sortMedicineList(result);
    this.setData({
      search_value: keyword,
      medicineSearch: result
    });
  },

  // 用户点击“取消”按钮
  onCancel() {
    this.setData({
      search_value: '',
      medicineSearch: this.data.medicineList
    });
  },

  // 删除卡片
  deleteCard(e) {
    const index = e.currentTarget.dataset.index;
    const deletedItem = this.data.medicineSearch[index];
  
    wx.showModal({
      title: '⚠确认删除操作',
      content: `确定要删除药物「${deletedItem.name}」吗？`,
      success: (res) => {
        if (res.confirm) {
          const newMedicineList = this.data.medicineList.filter(item => item.number !== deletedItem.number);
          const newSearchList = this.data.medicineSearch.filter((_, i) => i !== index);
          this.setData({
            medicineSearch: newSearchList,
            medicineList: newMedicineList
          });
          wx.setStorageSync('medicineInfoList', newMedicineList);
  
          // 向开发板发送删除指令
          const topic = `${this.data.productKey}/WX-app-device/user/post`;
          const deleteMessage = {
            "cmd": "DeleteMedicineInfo",
            "data": { "number": 1 }
          };
  
          if (this.data.client && this.data.client.connected) {
            this.data.client.publish(topic, JSON.stringify(deleteMessage), { qos: 0 }, (err) =>{
              if (err) {
                console.error("删除同步失败:", err);
                wx.showToast({ title: '同步失败', icon: 'none' });
              } else {
                console.log("删除同步成功:", deleteMessage);
                this.logOperation(`删除药物「${deletedItem.name}」成功`);
              }
            });
          }
        }
      }
    });
  },

  // 修改卡片信息
  editCard(e) {
    const index = e.currentTarget.dataset.index;
    const item = this.data.medicineSearch[index];
  
    wx.showModal({
      title: '⚠确认修改操作',
      content: `是否要修改药物「${item.name}」的信息？`,
      success: (res) => {
        if (res.confirm) {
          const take_timeStr = item.take_time.join(' ');
          this.setData({
            editIndex: index,
            editData: {
              name: item.name,
              take_timeStr: take_timeStr,
              amount: item.amount,
              taken_times: item.taken_times,
              number: item.number
            },
            showEditDialog: true
          });
        }
      }
    });
  },

  onInputChange(e) {
    const field = e.currentTarget.dataset.field;
    const value = e.detail;
    this.setData({
      editData: {
        ...this.data.editData,
        [field]: value
      }
    });
  },

  onEditConfirm() {
    const data = this.data.editData;
  
    // --- 输入校验 ---
    if (!data.name.trim()) {
      wx.showToast({ title: '药品名称不能为空', icon: 'none' }); return;
    }
    if (!data.take_timeStr.trim()) {
      wx.showToast({ title: '请输入服药时间', icon: 'none' }); return;
    }
    if (!String(data.amount).trim()) {
      wx.showToast({ title: '请输入剂量', icon: 'none' }); return;
    }
    if (data.taken_times === '' || data.taken_times === null) {
      wx.showToast({ title: '请输入已服次数', icon: 'none' }); return;
    }
    if (!String(data.number).trim()) {
      wx.showToast({ title: '请输入格子编号', icon: 'none' }); return;
    }
  
    // --- 处理时间字符串 ---
    let timeList = [];
    if (data.take_timeStr && typeof data.take_timeStr === 'string') {
      timeList = data.take_timeStr.trim().split(/\s+/).map(t => {
        if (!t.includes(':')) return ''; // 非合法时间跳过
        const [h, m] = t.split(':').map(Number);
        if (isNaN(h) || isNaN(m)) return ''; // 防止非数字报错
        const hh = h.toString().padStart(2, '0');
        const mm = m.toString().padStart(2, '0');
        return `${hh}:${mm}`;
      }).filter(t => t); // 过滤空字符串
    }
  
    const newItem = {
      name: data.name.trim(),
      take_time: timeList,
      amount: String(data.amount).trim(),
      taken_times: Number(data.taken_times),
      number: String(data.number).trim()
    };
  
    let medicineList = this.data.medicineList;
    if (this.data.isAddMode) {
      // 添加模式
      medicineList.push(newItem);
  
      // --- 同步添加的药物信息到开发板 ---
      const topic = `${this.data.productKey}/WX-app-device/user/post`;
      const syncData = {
        ...newItem,
        amount: Number(newItem.amount),
        taken_times: Number(newItem.taken_times),
        number: Number(newItem.number)
      };
      const syncMessage = {
        cmd: 'SyncMedicineInfo',
        data: syncData
      };
      
      if (this.data.client && this.data.client.connected) {
        this.data.client.publish(topic, JSON.stringify(syncMessage), { qos: 0 }, (err) => {
          if (err) {
            console.error("修改同步失败:", err);
            wx.showToast({ title: '同步失败', icon: 'none' });
          } else {
            console.log("修改同步成功:", syncMessage);
            wx.showToast({ title: '修改并同步成功', icon: 'success' });
            this.logOperation(`添加药物「${newItem.name}」成功`);
          }
        });
      }
  
    } else {
      // 修改模式
      const index = this.data.editIndex;
      if (index !== null) {
        medicineList[index] = newItem;
  
        // --- 同步修改后的信息 ---
        const topic = `${this.data.productKey}/WX-app-device/user/post`;
        const syncData = {
          ...newItem,
          amount: Number(newItem.amount),
          taken_times: Number(newItem.taken_times),
          number: Number(newItem.number)
        };
        const syncMessage = {
          cmd: 'SyncMedicineInfo',
          data: syncData
        };
        if (this.data.client && this.data.client.connected) {
          this.data.client.publish(topic, JSON.stringify(syncMessage), { qos: 0 }, (err) => {
            if (err) {
              console.error("修改同步失败:", err);
              wx.showToast({ title: '同步失败', icon: 'none' });
            } else {
              console.log("修改同步成功:", syncMessage);
              wx.showToast({ title: '修改并同步成功', icon: 'success' });
              this.logOperation(`修改药物「${newItem.name}」成功`);
            }
          });
        }
      }
    }
    this.sortMedicineList(medicineList);
    this.setData({
      medicineList,
      medicineSearch: medicineList,
      showEditDialog: false,
      editIndex: null,
      isAddMode: false
    });
    wx.setStorageSync('medicineInfoList', medicineList);
  },

  /*手动添加药物信息函数*/
  onAddCard() {
    this.setData({
      isAddMode: true, // 切换为添加模式
      showEditDialog: true,
      editData: {
        name: '',
        take_timeStr: '',
        amount: '',
        taken_times: '',
        number: ''
      },
      editIndex: null
    });
  },

  onEditCancel() {
    this.setData({
      showEditDialog: false, // 关闭编辑弹窗
      editIndex: null, // 重置编辑索引
      isAddMode: false, // 取消添加模式
      editData: {
        name: '',
        take_timeStr: '',
        amount: '',
        taken_times: '',
        number: ''
      } 
    });
  },
  /*操作记录函数*/
  logOperation(text) {
    const logs = wx.getStorageSync('operationLogs') || [];
    const time = new Date().toLocaleString(); // 格式: 2025/6/10 上午10:45:01
    logs.push({ text, time });
    wx.setStorageSync('operationLogs', logs);
  }
})