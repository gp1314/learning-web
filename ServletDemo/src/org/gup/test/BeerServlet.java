package org.gup.test;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.List;

public class BeerServlet extends HttpServlet {
    @Override
    protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {

    }

//    @Override
//    protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
//        resp.setContentType("text/html");
//        PrintWriter out = resp.getWriter();
//        out.println("Beer Selection Advice<br>");
//        String color = req.getParameter("color");
//        out.println("<br>Got beer color: " + color);
//    }

    @Override
    protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
        String color = req.getParameter("color");
        BeerExport beerExport = new BeerExport();
        List<String> retList = beerExport.doRecommended(color);
        req.setAttribute("styles",retList);
        RequestDispatcher view = req.getRequestDispatcher("result.jsp");
        view.forward(req,resp);

    }

}
