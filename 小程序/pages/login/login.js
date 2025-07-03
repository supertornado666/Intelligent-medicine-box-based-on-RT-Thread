

Page({
  data: {
    username: '',
    password: '',
    correctUser: 'rtt',
    correctPass: '123456'
  },

  onInputUsername(e) {
    this.setData({
      username: e.detail.value
    });
  },
  onInputPassword(e) {
    this.setData({
      password: e.detail.value
    });
  },
  

  onLogin() {
    if (this.data.username === this.data.correctUser && this.data.password === this.data.correctPass) {
      // 保存登录状态
      wx.setStorageSync('isAuthed', true);
      wx.showToast({ title: '登录成功', icon: 'success' });
      setTimeout(() => {
        wx.reLaunch({ url: '/pages/index/index' });
      }, 800);
    } else {
      wx.showToast({ title: '账号或密码错误', icon: 'none' });
    }
  }
});