//; var g_site_domain = "kaiwuonline.com";
; var mg = mg || {};
mg.sam = {
	version: "1.0",

	_loginHtml: [
		'<ul>',
		  '<li><input type="text" id="loginu" /></li>',
		  '<li><input type="password" id="loginp" /></li>',
		  '<li><input type="button" id="logins" value="提交" /></li>',
		'</ul>'
	].join(''),
	
	init: function() {
		var o = mg.sam;

		if (o.inited) return o;
		o.inited = true;

		o.outer = $('#logouts');
		
		return o;
	},

	detect: function() {
		var o = mg.sam.init();

		$.getJSON('http://www.mangoq.com/cgi-bin/login?jsoncallback=?', function(data) {
			if (data.success == '1') {
				$('#hint').html('登录域验证成功, samuser: ' + data.samuser +
								'samkey: ' + data.samkey);
				o.postLogin(data);
			} else if (data.errcode == '1') {
				$('#hint').html('<p>登录主域下验证失败，请重新登录</p>').append(o._loginHtml);
			} else if (data.errcode == '4') {
				$('#hint').html('<p>输入有误，请重新登录</p>').append(o._loginHtml);
			} else {
				$('#hint').html('<p>登录出错，请重新登录</p>').append(o._loginHtml);
			}
			$('#logins').click(mg.sam.login);
		});
	},

	onready: function() {
		var o = mg.sam.init();

		o.outer.click(o.logout);
	},
	
	login: function() {
		var o = mg.sam.init();

		var u = $('#loginu').val();
		var p = $('#loginp').val();

		if (!u.length || !p.length) {
			alert('请填写用户名密码。');
			return;
		}

		$.getJSON('http://www.mangoq.com/cgi-bin/login?jsoncallback=?&u='+u+'&p='+p, function(data) {
			if(data.success == '1') {
				$('#hint').html('登录域验证成功, samuser: ' + data.samuser +
								'samkey: ' + data.samkey);
				o.postLogin(data);
			} else if (data.errcode == '1') {
				alert('缺少参数');
			} else if (data.errcode == '4') {
				alert('密码错误');
			} else if (data.errcode == '5') {
				alert('过于频繁，稍后再试');
			} else {
				alert('登录出错');
			}
		});
	},

	logout: function() {
		var o = mg.sam.init();

		$.getJSON('http://www.mangoq.com/cgi-bin/logout?jsoncallback=?', function(data) {
			if (data.success == '1') {
				$('#hint').html('登出成功');
				o.postLogout();
			} else {
				alert('登出失败');
			}
		});
	},
	
	postLogin: function(data) {
		//$.cookie('samkey', data.samkey, { expires: 7, path: '/'});
		//$.cookie('samuser', data.samuser, { expires: 7, path: '/'});
		$.cookie('samkey', data.samkey, {path: '/'});
		$.cookie('samuser', data.samuser, {path: '/'});
	},

	postLogout: function() {
		$.cookie('samkey', null);
		$.cookie('samuser', null);
	}
};

$(document).ready(mg.sam.onready);
