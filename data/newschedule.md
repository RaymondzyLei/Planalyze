对于所有任务，都应该有<type>,<gid>,<rid>,<title>,<description>,<date>,<priority>,<category>,<completed>
其中，<type>有三种，分别为"point","schedule","deadline"
其中，"point"和"deadline"只有<time>，"schedule"有<startTime>和<endTime>，格式均为"08:30"。
对于"schedule"，如果结束时间比开始时间早，则认为是贯穿两天的事件，保证事件的时长不超过24h
<gid>表示general id，是指一组事件的编号，为数字
<rid>表示repeat id，是指一组事件中的某一个，为数字
值得注意的是，为了方便，对于没有组的单个事件而言，认为其成一个组。其<gid>正常并且<rid>为1即可
<title>是名字,<description>是描述，为中英文字符串
<date>表示日期，格式为"2025-10-30"，对于贯穿两天的事件，<date>表示事件的第一天的日期
<priority>为"high","medium","low"
<category>为分类，是中英文字符串
<compeleted>为true或者false