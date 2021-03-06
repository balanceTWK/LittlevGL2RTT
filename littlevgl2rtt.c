#include "littlevgl2rtt.h" 

static struct rt_device_graphic_info info; 

static void lcd_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > info.width  - 1) return;
    if(y1 > info.height - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > info.width  - 1 ? info.width  - 1 : x2;
    int32_t act_y2 = y2 > info.height - 1 ? info.height - 1 : y2;

    uint32_t x;
    uint32_t y;

    long int location = 0;

    if(info.bits_per_pixel == 16) 
    {
        uint16_t *fbp16 = (uint16_t*)info.framebuffer;
        for(x = act_x1; x <= act_x2; x++) 
        {
            for(y = act_y1; y <= act_y2; y++) 
            {
                location = (x) + (y) * info.width;
                fbp16[location] = color.full;
            }
        }
    }
}

static void lcd_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > info.width  - 1) return;
    if(y1 > info.height - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > info.width  - 1 ? info.width  - 1 : x2;
    int32_t act_y2 = y2 > info.height - 1 ? info.height - 1 : y2;

    long int location = 0;

    if(info.bits_per_pixel == 16) 
    {
        uint16_t *fbp16 = (uint16_t*)info.framebuffer;
        uint32_t x;
        uint32_t y;
        for(y = act_y1; y <= act_y2; y++) 
        {
            for(x = act_x1; x <= act_x2; x++) 
            {
                location = (x) + (y) * info.width;
                fbp16[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }
}

static void lcd_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > info.width  - 1) return;
    if(y1 > info.height - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > info.width  - 1 ? info.width  - 1 : x2;
    int32_t act_y2 = y2 > info.height - 1 ? info.height - 1 : y2;

    long int location = 0;

    /*16 bit per pixel*/
    if(info.bits_per_pixel == 16) 
    {
        uint16_t *fbp16 = (uint16_t*)info.framebuffer;
        uint32_t x;
        uint32_t y;
        
        for(y = act_y1; y <= act_y2; y++) 
        {
            for(x = act_x1; x <= act_x2; x++) 
            {
                location = (x) + (y) * info.width;
                fbp16[location] = color_p->full;
                color_p++;
            }

            color_p += x2 - act_x2;
        }
    }
    
    lv_flush_ready();
}

static void lvgl_tick_run(void *p)
{
    while(1)
    {
        lv_tick_inc(1); 
        rt_thread_delay(1);
    }
} 

rt_err_t littlevgl2rtt_init(const char *name) 
{
    RT_ASSERT(name != RT_NULL); 
    
    lv_init(); 
    
    /* LCD device */
    rt_device_t device = rt_device_find(name); 
    rt_device_open(device, RT_DEVICE_OFLAG_RDWR); 
    rt_device_control(device, RTGRAPHIC_CTRL_GET_INFO, &info); 
    
    /* littlevGZL */
    lv_disp_drv_t disp_drv; 
    lv_disp_drv_init(&disp_drv); 
    
    disp_drv.disp_fill  = lcd_fill;
    disp_drv.disp_map   = lcd_map;
    disp_drv.disp_flush = lcd_flush;

    lv_disp_drv_register(&disp_drv); 
    
    rt_thread_t thread = RT_NULL; 
    
    /* littlevGL tick thread */ 
    thread = rt_thread_create("lv_tick", lvgl_tick_run, RT_NULL, 512, 12, 10); 
    if(thread == RT_NULL)
    {
        return RT_ERROR; 
    }
    rt_thread_startup(thread);
    
    return RT_EOK; 
}
