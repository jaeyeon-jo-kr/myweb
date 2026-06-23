package com.example.demo;
import org.springframework.messaging.handler.annotation.MessageMapping;
import org.springframework.messaging.handler.annotation.SendTo;
import org.springframework.stereotype.Controller;

@Controller
public class ResourceInfoController {

    private final SystemStatusManager statusManager;

    public ResourceInfoController(SystemStatusManager statusManager) {
        this.statusManager = statusManager;
    }

    @MessageMapping("/info")
    @SendTo("/topic/system_list")
    public String SystemList()
    {
       return statusManager.getSystemList();
    }
}
