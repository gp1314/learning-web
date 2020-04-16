package org.gup.test;

import java.util.ArrayList;
import java.util.List;

public class BeerExport {
    public List<String> doRecommended(String color){
        List<String> retList = new ArrayList<>();
        if(color.equals("dark")){
            retList.add("奥丁格");
            retList.add("艾斯特");
        }else{
            retList.add("科罗拉");
            retList.add("树屋");
        }
        return retList;
    }
}
