// packageA/brief_introduction/brief_introduction.js
Page({
  data: {
      pictureList:[
        {
          img:"/packageA/sub_images/real_picture1.jpg",
          idx:0,
        },
        {
          img:"/packageA/sub_images/real_picture2.jpg",
          idx:1,
        },
        {
          img:"/packageA/sub_images/real_picture3.jpg",
          idx:2,
        },
        {
          img:"/packageA/sub_images/real_picture4.jpg",
          idx:3,
        },
      ],
  },

  goBack() {
      wx.navigateBack({
        delta: 1
      });
  },  
})