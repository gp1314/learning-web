<%--
  Created by IntelliJ IDEA.
  User: gupeng
  Date: 2020/3/29
  Time: 下午2:21
  To change this template use File | Settings | File Templates.
--%>
<%@ page contentType="text/html;charset=UTF-8" language="java" import="java.util.*" %>

<html>
<head>
    <title>Beer Demo</title>
</head>
<body>
<h1 align="center">Beer Recommendations JSP</h1>
<p>
    <%
        List<String> retList = (List)request.getAttribute("styles");
        for(String name:retList){
            out.print("<br>try: " + name);
        }
    %>
</p>
</body>
</html>
