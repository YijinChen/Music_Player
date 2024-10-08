0. bind
app: {"cmd": "bind", "appid": "101", "deviceid": "001"};
server: {"cmd": "bind_sucess"};

0. app check if it is already binded
app: {"cmd": "search_bind", "appid": "101"};
server: {"cmd": "reply_bind", "result": "yes"};  no

1. keep alive
client: {"cmd": "info", "status": "alive", "deviceid" : "001"};

2. start to play
app: {"cmd": "app_start"};
server to client: {"cmd": "start"};
client: {"cmd": "reply", "result": "start_success"};
server to app: {"cmd": "app_reply", "result": "start_success", "music": "xx.mp3", "voice": "50"}; 
               {"cmd": "app_reply", "result": "offline"};     

3. stop playing
app: {"cmd": "app_stop"};
server to client: {"cmd": "stop"};
client: {"cmd": "reply", "result": "stop_success"};
server to app: {"cmd": "app_reply", "result": "stop_success"};

4. suspend playing
app: {"cmd": "app_suspend"};
server to client: {"cmd": "suspend"};
client: {"cmd": "reply", "result": "suspend_success"};
server to app: {"cmd": "app_reply", "result": "suspend_success"};

5. continue playing
app: {"cmd": "app_continue"};
server to client: {"cmd": "continue"};
client: {"cmd": "reply", "result": "continue_success"};
server to app: {"cmd": "app_reply", "result": "continue_success"};

6. play prior one
app: {"cmd": "app_prior"};
server to client: {"cmd": "prior"};
client: {"cmd": "reply", "result": "success"};
server to app: {"cmd": "app_reply", "result": "success"};

7. play next one
app: {"cmd": "app_next"};
server to client: {"cmd": "next"};
client: {"cmd": "reply", "result": "success"};
server to app: {"cmd": "app_reply", "result": "success"};

8. increase volume
app: {"cmd": "app_volume_up"};
server to client: {"cmd": "volume_up"};
client: {"cmd": "reply", "result": "success"};
server to app: {"cmd": "app_reply", "result": "success", "voice": "50"};
               {"cmd": "app_reply", result: off_line};

9. increase volume
app: {"cmd": "app_volume_down"};
server to client: {"cmd": "volume_down"};
client: {"cmd": "reply", "result": "success"};
server to app: {"cmd": "app_reply", "result": "success", "voice": "50"};

10. sequence mode
app: {"cmd": "app_sequence"};
server to client: {"cmd": "sequence"};
client: {"cmd": "reply", "result": "success"};
server to app: {"cmd": "app_reply", "result": "success"};

11. random mode
app: {"cmd": "app_random"};
server to client: {"cmd": "random"};
client: {"cmd": "reply", "result": "success"};
server to app: {"cmd": "app_reply", "result": "success"};

12. circle mode
app: {"cmd": "app_circle"};
server to client: {"cmd": "circle"};
client: {"cmd": "reply", "result": "success"};
server to app: {"cmd": "app_reply", "result": "success"};

13. get player status
server: {"cmd": "get"};
client: {"cmd": "reply_status", "status": "start", "music": "xx.mp3", "voice": "50"}; start suspend stop
server to app: {"cmd": "app_reply_status", "status": "start", "music": "xx.mp3", "voice": "50"};

14. get music
app: {"cmd": "app_music"};
server to client: {"cmd": "music"};
client: {"cmd": "reply_music", "music": ["1.mp3", "2.mp3", "3.mp3"]};
server to app: {"cmd": "app_reply_music", "music": ["1.mp3", "2.mp3", "3.mp3"]};
(If the player is offline) server to app: {"cmd": "app_reply", "result": "off_line"};

15. app offline
app: {"cmd" : "app_off_line"};


keys:
1.start/continue
2.stop/suspend
3.prior
4.next
5.volume up
6.volume down
7.change mode
