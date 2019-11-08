/*
author: matt ji
date: 2019-8-29
brief: 关于wifi控制流程的函数接口
test on ubuntu 16.04
*/
/*1. 检查所有连接，看有没有特定连接(tikong-wifi)的名字 check_exist*/

#include "wifi-fun.h"
#include "wifi.h"
#include <gio/gio.h>
#include <uuid/uuid.h>
#include <string.h>
#include <nm-dbus-interface.h>
#include <NetworkManager.h>
#include <stdlib.h>
#include <stdio.h>

/*2. 创建连接函数 add_wifi_connection*/
int add_wifi_connection(const char *con_name)
{
    GDBusProxy *proxy;
	GError *error = NULL;

	/* Create a D-Bus proxy; NM_DBUS_* defined in nm-dbus-interface.h */
	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
	                                       NM_DBUS_SERVICE,
                                           //"/org/freedesktop/NetworkManager/Settings"
	                                       NM_DBUS_PATH_SETTINGS,
                                           // "org.freedesktop.NetworkManager.Settings"
	                                       NM_DBUS_INTERFACE_SETTINGS,
	                                       NULL, &error);
	if (!proxy) {
		g_dbus_error_strip_remote_error (error);
		g_print ("Could not create NetworkManager D-Bus proxy: %s\n", error->message);
		g_error_free (error);
		return 1;
	}

	/* Add a connection */
	//add_connection (proxy, con_name);

	g_object_unref (proxy);

	return 0;
}

/*3. 删除连接 remove_connection()*/
gboolean
remove_wifi_connection(const char *ssid)
{
    GDBusProxy *proxy;
    gboolean found = FALSE;

	/* Create a D-Bus proxy; NM_DBUS_* defined in nm-dbus-interface.h */
	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
	                                       NM_DBUS_SERVICE,
	                                       NM_DBUS_PATH_SETTINGS,
	                                       NM_DBUS_INTERFACE_SETTINGS,
	                                       NULL, NULL);
	g_assert (proxy != NULL);
    
    //remove_tk_wifi (proxy);
    int i;
	GError *error = NULL;
	GVariant *ret;
	char **paths;

	/* Call ListConnections D-Bus method */
	ret = g_dbus_proxy_call_sync (proxy,
	                              "ListConnections",
	                              NULL,
	                              G_DBUS_CALL_FLAGS_NONE, -1,
	                              NULL, &error);
	if (!ret) {
		g_dbus_error_strip_remote_error (error);
		g_print ("ListConnections failed: %s\n", error->message);
		g_error_free (error);
		return FALSE;
	}

	g_variant_get (ret, "(^ao)", &paths);
	g_variant_unref (ret);

	for (i = 0; paths[i]; i++)
    {        
        found = get_active_connection_details (paths[i], ssid);
        if(found == TRUE)
        {
            remove(paths[i]);
        }       
        
    }
	g_strfreev (paths);
    
	g_object_unref (proxy);

	return found;
}

/*4. 连接wifi函数 connect_wifi_fun()*/
 gboolean
connect_wifi_fun()
{
    GDBusProxy *proxy;
    gboolean found = FALSE;

	/* Create a D-Bus proxy; NM_DBUS_* defined in nm-dbus-interface.h */
	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
                                           // "org.freedesktop.NetworkManager"
	                                       NM_DBUS_SERVICE,
                                           //"/org/freedesktop/NetworkManager"
	                                       NM_DBUS_PATH,
                                           //"org.freedesktop.NetworkManager"
	                                       //NM_DBUS_INTERFACE,
	                                       "org.freedesktop.DBus.Properties",
	                                       NULL, NULL);
	g_assert (proxy != NULL);
    
	//enable_conn(proxy);
    
	g_object_unref (proxy);

    return TRUE;
}


 gboolean
check_exist (const char *ssid)
{
	GDBusProxy *proxy;
    gboolean found = FALSE;

	/* Create a D-Bus proxy; NM_DBUS_* defined in nm-dbus-interface.h */
	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
	                                       NM_DBUS_SERVICE,
	                                       NM_DBUS_PATH_SETTINGS,
	                                       NM_DBUS_INTERFACE_SETTINGS,
	                                       NULL, NULL);
	g_assert (proxy != NULL);

	/* List connections of system settings service */
    //check if exist    
	found = find_tk_wifi (proxy, ssid);
    if(found == TRUE)
    {
        //g_print("ok, find it\n");
        found = TRUE;
    }
    else
    {
        //创建连接
        //g_print("well, not found, create it then\n");
        found = FALSE;
        
    }
    
	g_object_unref (proxy);

	return found;
}



/*找到硬件路径*/
gboolean
find_hw()
{
    GDBusProxy *proxy;
    gboolean found = FALSE;

	/* Create a D-Bus proxy; NM_DBUS_* defined in nm-dbus-interface.h */
	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
                                           // "org.freedesktop.NetworkManager"
	                                       NM_DBUS_SERVICE,
                                           //"/org/freedesktop/NetworkManager"
	                                       NM_DBUS_PATH,
                                           //"org.freedesktop.NetworkManager"
	                                       //NM_DBUS_INTERFACE,
	                                       "org.freedesktop.DBus.Properties",
	                                       NULL, NULL);
	g_assert (proxy != NULL);
    
	found = find_hw_fun(proxy);
    
	g_object_unref (proxy);

    return found;
}


/*5. 断开wifi函数 disconn_wifi()*/
 int
//disconnect_wifi(const char *device_path)
disconnect_wifi(const char *iface, char *err)
{

	GDBusProxy *proxy;
    //gboolean found = FALSE;
    GError *error = NULL;
    //g_print("first diconn wifi\n");

	/* Create a D-Bus proxy; NM_DBUS_* defined in nm-dbus-interface.h */
	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
                                           // "org.freedesktop.NetworkManager"
	                                       NM_DBUS_SERVICE,
	                                       //device_path,
	                                       iface,                                          
	                                       "org.freedesktop.NetworkManager.Device",
	                                       NULL, NULL);
	g_assert (proxy != NULL);
	GVariant *ret = NULL;
    
	ret = g_dbus_proxy_call_sync (proxy,
                              "Disconnect",
                              NULL,
                              G_DBUS_CALL_FLAGS_NONE, -1,
                              NULL, &error);
    //const char *value = NULL; 
    //GVariant *value;
	//g_variant_get (ret, "(o)", &value);
	//g_print("return current %s\n", value);

	if (ret) {
		g_variant_unref (ret);
		g_object_unref (proxy);
		return 0;
	} else {
		g_dbus_error_strip_remote_error (error);
		g_print ("Error disconnet wifi: %s\n", error->message);
		strcpy(err, error->message);
		g_clear_error (&error); 
		g_object_unref (proxy);
		return 1;
	}

	//g_object_unref (ret);

    //return TRUE;
}


/*6. 测试网络连通性函数 check_connectivity()*/

//这里如果一连上就去ping会出现connect: Network is unreachable

 int
check_connectivity(const char *iface, char * err)
{
/*	int ret = -1;
	ret = system("ping 192.168.1.1");
//	ret = system("ping 192.168.1.1 -c 1");
	if(ret == 0)
	{
		strcpy(err, "network ok");
		return TRUE;
	}
	else
	{
		strcpy(err, "network not working");
		return FALSE;
	}
	//增加ping外网的需求，ping150主控
	*/
	//一种方法，一直去查询dbus的某一个值
	/*
	string cmd = "nmcli c add type wifi con-name tikong-wifi ifname ";
    	cmd.append(WLAN);
    	cmd.append(" ssid tikong");
        //system("nmcli c add type wifi con-name tikong-wifi ifname wlp3s0 ssid tikong");
        system(cmd.c_str());
	*/

	gboolean status;
	char cmd[100]="";
	strcpy(cmd, "ifconfig | grep ");
	strcat(cmd, iface);
	strcat(cmd, " -A1 | grep -v Link");
	//exec("ifconfig | grep wlp2s0 -A1 | grep -v Link", err);
	exec(cmd, err);
	//exec("ifconfig | grep wlp3s0 -A1 | grep -v Link");
	printf("%s\n", err);
	if(strstr(err, "192.168.1") != NULL)
	{
		return 0;
	}

	return 1;
	//这里应该提供信号强度
} 



//void connect_wifi(const char *ssid, char *ret)
int connect_wifi(const char *iface, const char *ssid, const char *pw, char *ret)
{ 
	
    gboolean status = FALSE;

    status = find_hw();
    if(status == TRUE)
    {
        //printf("found wifi hardware\n");  
        //strcpy(ret,"found wifi hardware"); 
    }
    else if(status == FALSE)
    {
        printf("no wifi hardware, return\n"); 
        strcpy(ret,"no wifi hardware"); //错误1
        return 1;
    }
    //check_exist函数里会把连接的PATH找到
    status = check_exist(ssid);
    if(status == TRUE)
    {
        //remove_wifi_connection(SSID);  
        //enable_conn(path);      
        printf("found wifi ssid\n");  
    }
    else if(status == FALSE)
    {
    	printf("need establish %s connection\n", ssid);
        status = add_connection (iface, ssid, pw,ret);
        if(status == FALSE)
	    {
	    	printf("%s\n", ret); //错误2 已经拷贝进去
	        return 2;
	    }
	    else if(status == TRUE)
	    {
	    	printf("add connection ok\n"); 
    	}
    }
    //enable wifi connection
    status = enable_conn(ret); //错误3
    if(status == TRUE)
    {
    	strcpy(ret,"connection enabled"); //正确1
    }
    else
    {
    	//错误信息已经在enable_conn函数里拷贝到ret里了  //错误3
    }
    //sleep(8);
    return 0;

}

int get_version(char *ver)
{
	strcpy(ver, "1.0");
	return 0;
}