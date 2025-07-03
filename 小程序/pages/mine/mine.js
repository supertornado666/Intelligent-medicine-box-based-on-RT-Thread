// pages/mine/mine.js
import Dialog from '@vant/weapp/dialog/dialog';
Page({
  onChangePassword() {
    wx.showToast({ title: '功能暂未开放', icon: 'none' });
  },

  onShowGuide() {
    wx.navigateTo({
      url: '/packageA/brief_introduction/brief_introduction'
    });
  },

  onClearStorage() {
    wx.showToast({ title: '功能暂未开放', icon: 'none' });
    //wx.clearStorageSync();
    //wx.showToast({ title: '缓存已清除', icon: 'success' });
  },

  onContact() {
    wx.showModal({
      title: '联系我们',
      content: `邮箱: xxxxxxx@xx.com\n微信: xxxxxxx`,
      showCancel: false
    });
  },

  onLogout() {
    wx.showModal({
      title: '退出登录',
      content: '确定要退出当前账号吗？',
      confirmText: '退出',
      cancelText: '取消',
      success: (res) => {
        if (res.confirm) {
          // 清除登录状态
          wx.removeStorageSync('isAuthed');
          wx.removeStorageSync('rememberedUser');
          wx.removeStorageSync('rememberedPass');
          // 清除更多缓存
          // wx.clearStorageSync();
          // 返回登录页面
          wx.reLaunch({
            url: '/pages/login/login'
          });
        }
      }
    });
  },

  onViewLogs() {
    wx.navigateTo({
      url: '/packageA/history/history'
    });
  }
});