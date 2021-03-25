/**
 * 用户消息框模板
 * @type {string}
 */
var article = `<article class="$[Type]">
                <div class="avatar">
                    <img alt="avatar" src="$[Avatar]" />
                </div>
                <div id="$[Id]" class="msg $[Article-Context-Class]">
                    <div class="tri"></div>
                    <div class="msg_inner">$[Message]</div>
                </div>
            </article>`;

/**
 * 系统消息框模板
 * @type {string}
 */
var systemArticle = `<div class="chat-notice" id="$[Id]">
                         <span>$[Message]</span>
                     </div>`;

/**
 * 他人消息昵称框模板
 * @type {string}
 */
var messageModel = `<nick style='color: #3fbcf2;font-size: medium;font-weight: bold'>$[Nick]</nick><br/><a class="other_msg" style="">$[Message]</a>`;

/**
 * 回复消息框模板
 * @type {string}
 */
var respondModel = `<div style="background-color: #9be1ff;color: #2f2f2f;font-size:medium;">
                        <nick style="font-size: small;font-weight: bolder"><a style="font-weight: lighter">$[Date]</a> >>> $[Nick]</nick><br/>
                        <a>$[Message]</a>
                    </div>`;

/**
 * 回复消息暂存区
 * @type {string}
 */
var respondMessage;

/**
 * 回复消息标识符
 * @type {string}
 */
var respondIdentification = '[回复]';

/**
 * 日期标准格式
 * @type {string}
 */
var timeType = 'yyyy年MM月dd日 hh:mm';

/**
 * 撤回功能锁定
 * @type {boolean}
 */
var isLockWithdraw = true;

/**
 * 回复功能锁定
 * @type {boolean}
 */
var isLockReplay = true;

/**
 * 上次消息（用户消息）时间
 * @type {number}
 */
var messageTime;

/**
 * 日志文件格式
 * server：服务器网关主机
 * room:聊天室/私聊对象ID
 * myNick:我的昵称
 * avatar:头像Id
 * createDate:日志创建时间
 * lastUpdateDate:最后一次修改时间
 * messages:消息序列
 */
var fileLog = {
    "server":null,
    "room":null,
    "myNick":null,
    "avatar":null,
    "createDate":0,
    "lastUpdateDate":0,
    "messages":[]
};

/**
 * Message内的单条消息格式
 * @type {{date: number, nick: null, msg: null, avatar: null, type: number}}
 */
var fileLogModel = {
    "date":0,
    "type":0,
    "nick":null,
    "msg":null,
    "avatar":null
};

/**
 * 消息类型
 * 系统消息：0
 * 我的消息：1
 * 他人消息：2
 */
var messageType = {
    System : 0,
    Me : 1,
    Other : 2
};

var qtApiCommand = {
    SendMessage : 1
};

/**
 * QMessageBox类型
 * 标明不同的异常等级
 */
var qMessageBoxType = {
    Info : 0,
    Warning : 1,
    Error : 2
};

/**
 * 获取日志信息
 * @returns {string} 当前的所有消息记录
 */
function getFileLog() {
    return JSON.stringify(fileLog);
}

/**
 * 新消息添加至日志中
 * @param date 消息时间
 * @param type 消息类型:messageType
 * @param message 消息内容
 * @param nick 昵称
 * @param avatar 头像，Null为默认
 */
function addFileLog(date,type, message, nick, avatar) {
    var log = {};
    log.date = date;
    log.type = type;
    log.msg = message;
    log.nick = nick;
    log.avatar = avatar;
    fileLog.messages.push(log);
    fileLog.lastUpdateDate = new Date().getTime();
}

/**
 * 获取消息框的 Html代码
 * @param message 消息
 * @param type 类型
 * @param avator 头像
 * @param id 用户Id
 * @returns {string} 消息框的Html字符串
 */
function getArticle(message,type,avator,id) {
    if (avator === undefined) {
        avator = './unknown.jpg';
    }
    var context = 'message-context-other';
    if (type === 'right') context = 'message-context-me';
    return article.replace("$[Type]",type)
        .replace("$[Avatar]",avator)
        .replace("$[Message]",message)
        .replace("$[Article-Context-Class]",context)
        .replace("$[Id]",id);
}

/**
 * 获取系统消息框的Html代码
 * @param message 消息
 * @param id 系统消息ID
 * @returns {string} 消息框的Html代码
 */
function getSystemArticle(message,id) {
    return systemArticle.replace("$[Message]",message)
        .replace("$[Id]",id);
}

/**
 * 将指定的组件的位置移动至最下
 * @param assembly 组件筛选，规则同JQuery组件选择器
 */
function rollToEnd(assembly){
    var messages = $(assembly);
    messages.scrollTop(messages.prop("scrollHeight"));
}

/**
 * 时间格式化方法
 * @param fmt 时间格式字符串
 * @returns {void | string}
 */
Date.prototype.format = function (fmt) {
    var o = {
        "M+": this.getMonth() + 1, //月份
        "d+": this.getDate(), //日
        "h+": this.getHours(), //小时
        "m+": this.getMinutes(), //分
        "s+": this.getSeconds(), //秒
        "q+": Math.floor((this.getMonth() + 3) / 3), //季度
        "S": this.getMilliseconds() //毫秒
    };
    if (/(y+)/.test(fmt)) fmt = fmt.replace(RegExp.$1, (this.getFullYear() + "").substr(4 - RegExp.$1.length));
    for (var k in o)
        if (new RegExp("(" + k + ")").test(fmt)) fmt = fmt.replace(RegExp.$1, (RegExp.$1.length === 1) ? (o[k]) : (("00" + o[k]).substr(("" + o[k]).length)));
    return fmt;
};

/**
 * 检查与之前的消息时间是否超过了5分钟
 * 超过5分钟触发一条时间消息显示当前
 */
function checkDate() {
    var date = new Date();
    if((date.getTime() - messageTime)>300000){
        systemDate(date);
    }
    messageTime = date.getTime();
}

/**
 * 新消息-底层方法
 * @param message 消息内容
 * @param type 消息类型，需要是messageType中的一个
 * @param id 消息Id
 * @param avatar 头像Url
 */
function newMessage(message,type,id,avatar) {
    var messages = $('#messages');
    switch (type) {
        case 0:
            //系统消息
            messages.append(getSystemArticle(message,id));
            rollToEnd('#messages');
            return;
        case 1:
            checkDate();
            messages.append(getArticle(message,'right',avatar,id));
            rollToEnd('#messages');
            return;
        case 2:
            checkDate();
            messages.append(getArticle(message,'',avatar,id));
            rollToEnd('#messages');
            return;
        default:
            throw new Error('Type ['+type+'] is not a message type');
    }
}

/**
 * 重写默认的alert方法，将消息封装至外部 Qt程式
 * @param message 消息
 * @param type 消息类型，默认为Info级别
 */
function alert(message,type) {
    if (type === undefined) type = qMessageBoxType.Info;
    window.bridge_js.alert(message,type);

}

/**
 * 用户消息
 * @param type 消息类型，为messageType中
 * @param message 消息
 * @param nick 昵称
 * @param avatar 头像Url
 * @param date 日期
 */
function userMessage(type,message,nick,avatar,date) {
    if(type === messageType.Me || type === messageType.Other) {
        if (date === undefined||date === null) {
            date = new Date().getTime();
        }
        if (avatar === null) {
            avatar = './unknown.jpg';
        }
        addFileLog(date,type,message,nick,avatar);
        switch (type) {
            case messageType.Other:
                var style = '';
                if (fileLog.myNick !== null && fileLog.myNick !== undefined ) {
                    if (message.indexOf('@'+fileLog.myNick) !== -1) {
                        style = ' style="color:red"';
                    }
                }
                newMessage(
                    messageModel
                        .replace("$[Nick]", nick)
                        .replace("$[Message]", message)
                        .replace(' style=""',style),
                    messageType.Other,
                    date,
                    avatar
                );
                return;
            case messageType.Me:
                newMessage(message, messageType.Me,date, avatar);
                return;
        }
    }
}

/**
 * 系统消息
 * @param message 消息内容
 * @param date 消息时间
 */
function systemMessage(message,date) {
    if (date === undefined||date === null) date = new Date().getTime();
    addFileLog(date,messageType.System,message);
    newMessage(message,messageType.System,date);
}

/**
 * 发送消息
 * 该函数为测试函数，正式环境将被重写
 */
function send(messageValue){

    var message = $('#message');
    if (message.val().indexOf(respondIdentification) === 0) {
        if (messageValue.replace(respondIdentification, '') === '') return;
        if (respondMessage !== null){
            respondMessage = respondMessage.substring(0,respondMessage.indexOf('</div>')+6);
            messageValue = respondMessage + messageValue.replace(respondIdentification, '');
        }
    }

    respondMessage = null;

    //追加消息
    userMessage(messageType.Me,messageValue,fileLog.myNick,fileLog.avatar);
}

/**
 * 系统消息
 * @param date
 */
function systemDate(date) {
    newMessage(date.format(timeType),messageType.System)
}

/**
 * \@其他人
 * @param somebody 其他人昵称
 */
function atOther(somebody){
    somebody = '@'+somebody;
    var message = $('#message');
    if (message.val().indexOf(somebody) === 0) return;
    message.val(somebody+' '+message.val());
}

/**
 * 清空当前屏幕的全部消息内容
 */
function empty(){
    $('#messages').html('');
    messageTime = 0;
}

/**
 * 初始化Web组件
 * @param server 服务器地址
 * @param myNick 我的昵称
 * @param avatar 头像
 */
function init(server,myNick,avatar){
    fileLog.server = server;
    fileLog.myNick = myNick;
    fileLog.avatar = avatar;
    fileLog.createDate = new Date().getTime();
}

/**
 * 加载历史消息
 * @param historyLogFile 历史消息
 */
function loadHistory(historyLogFile){
    fileLog = JSON.parse(historyLogFile);
    var messages = fileLog.messages;
    for (var i = 0;i < messages.length;i++) {
        var msg = messages[i];
        if (i === 0) {
            var date = new Date();
            date.setTime(msg);
            systemDate(date);
        }
        if (msg.type === messageType.Other){
            newMessage(
                messageModel
                    .replace("$[Nick]", msg.nick)
                    .replace("$[Message]", msg.msg),
                messageType.Other,
                msg.avator
            );
        }else {
            newMessage(msg.msg,msg.type,msg.avator);
        }
    }
}

/**
 * 保存历史消息，触发Qt存储函数
 */
function saveHistory(){
    window.bridge_js.saveHistory(getFileLog());
}

/**
 * 回复消息
 * @param id 回复消息Id
 * @param nick 回复消息昵称
 * @param talk 回复消息内容
 */
function respondMessageFun(id, nick, talk) {
    var message = $('#message');
    var date = new Date();
    date.setTime(parseInt(id));
    respondMessage = respondModel
            .replace('$[Date]',date.format(timeType))
            .replace("$[Nick]",nick)
            .replace("$[Message]",talk)
        + message.val();
    if (message.val().indexOf(respondIdentification) === 0){
        return;
    }
    message.val(respondIdentification+message.val());
    message.scrollTop(message.prop("scrollHeight"));
}

/**
 * 回复他人消息，由组件调用回调
 * @param key
 * @param options
 * @param e
 */
function respond(key, options, e) {

    var id = options.$trigger.attr('id');
    var nick = options.$trigger.children("div").children("nick").html();
    var talk = options.$trigger.children("div").children("a").html();
    respondMessageFun(id,nick,talk)

}

/**
 * 回复自己消息，由组件调用回调
 * @param key
 * @param options
 * @param e
 */
function respondMe(key, options, e) {

    var id = options.$trigger.attr('id');
    var nick = fileLog.myNick;
    if (nick === null) nick = '我';
    var talk = options.$trigger.children('.msg_inner').html();
    if (talk.indexOf('</div>') !== -1) {
        talk = talk.substring(talk.indexOf('</div>')+6);
    }
    respondMessageFun(id,nick,talk)

}

/**
 * 他人消息右键菜单回调函数
 * @param key
 * @param options
 * @param e
 */
function contextOther(key, options,e) {
    var nick = options.$trigger.children("div").children("nick").html();
    var talk = options.$trigger.children("div").children("a").html();
    switch (key) {
        case 'copy':
            var $temp = $("<input/>");
            $("body").append($temp);
            $temp.val(talk).select();
            document.execCommand("copy");
            $temp.remove();
            return;
        // case 'data':
        //     return;
        case 'at':
            var message = $('#message');
            if (message.val().indexOf('@'+nick+' ') !== -1) return;
            message.val('@'+nick+' '+message.val());
            return;
        case 'respond':
            if(isLockReplay){
                systemMessage('回复功能已锁定');
                return;
            }
            respond(key,options,e);
            return;
        default:
            systemMessage('功能正在维护中...');
            return;
    }
}

/**
 * 个人消息右键菜单回调函数
 * @param key
 * @param options
 * @param e
 */
function contextMe(key,options,e) {
    var talk = options.$trigger.children('.msg_inner').html();
    switch (key) {
        case 'withdraw':
            if(isLockWithdraw) {
                systemMessage('撤回功能已锁定');
                return;
            }
            //撤回逻辑
            options.$trigger.parent().remove();
            systemMessage('我 撤回了一条消息');
            return;
        case 'copy':
            var $temp = $("<input/>");
            $("body").append($temp);
            if (talk.indexOf('</div>') !== -1) talk = talk.substring(talk.indexOf('</div>')+6);
            $temp.val(talk).select();
            document.execCommand("copy");
            $temp.remove();
            return;
        case 'respond':
            if(isLockReplay){
                systemMessage('回复功能已锁定');
                return;
            }
            respondMe(key, options, e);
            return;
        default:
            systemMessage('功能正在维护中...');
            return;
    }
}

function sendMessageApi(){
    var message = $('#message');
    if (message.val().replace('\n','') === ''){
        message.val('');
        return;
    }

    if(!isLockReplay){
        if (message.val().indexOf(respondIdentification) === 0) {
            if (messageValue.replace(respondIdentification, '') === '') return;
            if (respondMessage !== null){
                respondMessage = respondMessage.substring(0,respondMessage.indexOf('</div>')+6);
                messageValue = respondMessage + messageValue.replace(respondIdentification, '');
            }
        }
    }
    respondMessage = null;


    window.bridge_js.chatApis(qtApiCommand.SendMessage,message.val());

    //清空消息框
    message.val('');
}

/**
 * 初始化函数
 */
$(function () {

    //初始化消息时间
    var date = new Date();
    systemDate(date);
    messageTime = date.getTime();

    // 初始化右键菜单
    $(function() {
        $.contextMenu({
            selector: '.message-context-other',
            callback: function(key, options,e) {
                // var m = "clicked: " + key;
                // window.console && console.log(m);
                // console.log(options.$trigger.attr("id"));
                // console.log(e);
                contextOther(key,options,e);
            },
            items: {
                // "edit": {name: "Edit", icon: "edit"},
                // "cut": {name: "Cut", icon: "cut"},
                // copy: {name: "Copy", icon: "copy"},
                // "paste": {name: "Paste", icon: "paste"},
                // "delete": {name: "Delete", icon: "delete"},
                // "sep1": "---------",
                // "quit": {name: "Quit", icon: function(){
                //         return 'context-menu-icon context-menu-icon-quit';
                //     }}
                "copy":{name:"复制"},
                "data":{name:"Ta的个人资料"},
                "at":{name:"@Ta"},
                "respond":{name:"回复"},
                "pipeline":{name:"私聊"}
            }
        });

        $.contextMenu({
            selector: '.message-context-me',
            callback: function(key, options,e) {
                 contextMe(key,options,e);
            },
            items: {
                "copy":{name:"复制"},
                "forward":{name:"转发"},
                "withdraw":{name:"撤回"},
                "respond":{name: "回复"}
            }
        });
    });

    //userMessage(messageType.Other,'看见好处就要抢',"雷狮"/*,"../icon/leishi.jpg"*/);

    //创建到 Qt程序 的连接，创建完成之后初始化Web组件
    new QWebChannel(qt.webChannelTransport, function(channel) {
        window.bridge_js = channel.objects.bridge;
        window.bridge_js.init();
    });
});

//发送按钮监听
$('#send').click(function () {
    sendMessageApi();
});

//发送回车键监听
$('#message').keyup(function (event) {
    if (event.keyCode === 13) {
        sendMessageApi();
    }
});

//文件选择按键监听
$('#file').click(function () {
    //newMessage(getFileLog(),messageType.Other);
    window.bridge_js.file();
});
