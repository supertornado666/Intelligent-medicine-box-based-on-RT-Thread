Page({
  data: {
    logs: []
  },

  onLoad() {
    const logs = wx.getStorageSync('operationLogs') || [];
    this.setData({ logs: logs.reverse() }); // 倒序显示，最新在前
  },

  onClearLogs() {
    const that = this;
    wx.showModal({
      title: '确认清空',
      content: '确定要清空所有操作记录吗？此操作无法恢复。',
      confirmColor: '#d43c33',
      success(res) {
        if (res.confirm) {
          wx.removeStorageSync('operationLogs');
          that.setData({ logs: [] });
          wx.showToast({
            title: '已清空',
            icon: 'success'
          });
        }
      }
    });
  },
  goBack() {
    wx.navigateBack({
      delta: 1
    });
  },  
});