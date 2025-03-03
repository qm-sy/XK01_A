#include "KEY_CRL.h"

KEY key;

void key_init( void )
{
    key.key_init_flag = 1;

    key.gui_key2_allow_flag = 1;
    key.gui_key3_allow_flag = 1;
    key.gui_key4_allow_flag = 0;  
    key.sync_allow_flag = 0;
    key.sycn_keep_cnt = 0;
    key.key1_press_cnt = 0;
    key.key4_press_cnt = 0;
}

void jump_to_init( void )
{
    /*       按键2、3恢复调节风扇功率功能     */
    key.gui_key2_allow_flag = 1;
    key.gui_key3_allow_flag = 1;

    /*       按键2、3恢复调节风扇功率功能     */
    key.key1_press_cnt = 0;
    key.key4_press_cnt = 0;
    
    /*       无图标跳动     */
    gui_beat.beat_switch = 0;

    /*       可继续查询从机状态     */
    modbus.modbus_04_scan_allow = 1;

    /*       复位图标跳动起始点     */
    gui_beat.beat_select = KONG;

}

void key_scan( void )
{
    uint8_t key_value;
    uint8_t key4_delay_cnt = 100;

    /*       50ms检测一次     */
    if(key.key_value_flag == 1)
	{
		key_value = (B0_VAL) | (B1_VAL<<1) | (B2_VAL<<2) | (B3_VAL<<3);
	}
    switch (key_value)
    {
        case KEY1:      key1_press();       break;

        case KEY2:      key2_press();       break;

        case KEY3:      key3_press();       break;

        case KEY4:    
            while((key_value == KEY4)&&(key4_delay_cnt!=0)) 
            {
                key_value = (B0_VAL) | (B1_VAL<<1) | (B2_VAL<<2) | (B3_VAL<<3);
                key4_delay_cnt--;
                delay_ms(10); 
            }
            if( key4_delay_cnt == 0 )
            {
                if( gui_beat.beat_switch == 0 )
                {
                    key.sync_allow_flag = 1 - key.sync_allow_flag;
                    sync_switch();
                }
            }else
            {
                key4_press(); 
            }
                  
            break;

        default:                            break;
    }
}

void key1_press()
{
    buzzer_open();

    if( key.key1_press_cnt == KONG ) 
    {
        key.key1_press_cnt = 0;
    }

    switch (gui_beat.beat_select)
    {
        case NTC_TEMP1_STR:
            gui_beat.beat_select = NTC_TEMP2_STR;
            break;

        case NTC_TEMP2_STR:
            gui_beat.beat_select = NTC_TEMP3_STR;
            break;

        case NTC_TEMP3_STR:
            gui_beat.beat_select = NTC_TEMP1_STR;
            break;

        case BAKE_POWER_STR:
            gui_beat.beat_select = BAKE_WIND_STR;
            break;

        case BAKE_WIND_STR:
            gui_beat.beat_select = BAKE_POWER_STR;
            break;

        case FAN_LEVEL_STR:
            jump_to_init();
            break;
            
        default:
            key.key1_press_cnt++;

            gui_beat.beat_select = key.key1_press_cnt;
            gui_beat.beat_switch = BEAT_ON;

            key.gui_key2_allow_flag = 0;
            key.gui_key3_allow_flag = 0;

            modbus.modbus_04_scan_allow = 0;

            key.gui_key4_allow_flag = 1;
            break;
    }
    
    buzzer_close();
    key.key_value_flag = 0;
}

void key2_press()
{
    buzzer_open();

    if( gui_beat.beat_switch == 0 )
    {
        gui_info.fan_level += 1;
        if( gui_info.fan_level >= 6 )
        {
            gui_info.fan_level = 6;
        }
        LCD_ShowNum(135,165,gui_info.fan_level,1,32,POINT_COLOR,BACK_COLOR);
        write_slave_reg();
    }

    if( key.gui_key2_allow_flag == 1 )
    {
        switch(gui_beat.beat_select)
        {
            case NTC_TEMP1_STR:
                gui_info.ntc1_temp += 1;
                if( gui_info.ntc1_temp >= 120 )
                {
                    gui_info.ntc1_temp = 120;
                }
                break;

            case NTC_TEMP2_STR:
                gui_info.ntc2_temp += 1;
                if( gui_info.ntc2_temp >= 120 )
                {
                    gui_info.ntc2_temp = 120;
                }
                break;

            case NTC_TEMP3_STR:
                gui_info.ntc3_temp += 1;
                if( gui_info.ntc3_temp >= 120 )
                {
                    gui_info.ntc3_temp = 120;
                }
                break;

            case FAN_LEVEL_STR:
                gui_info.fan_level += 1;
                if( gui_info.fan_level >= 6 )
                {
                    gui_info.fan_level = 6;
                }
                break;

            case BAKE_POWER_STR:
                gui_info.bake_power_percentage += 5;
                if( gui_info.bake_power_percentage >= 100 )
                {
                    gui_info.bake_power_percentage = 100;
                }
                break;

            case BAKE_WIND_STR:
                gui_info.bake_wind_level += 1;
                if( gui_info.bake_wind_level >= 6 )
                {
                    gui_info.bake_wind_level = 6;
                }
                break;

            default:
                break;
        }
    }

    buzzer_close();
    key.key_value_flag = 0;
}

void key3_press()
{
    buzzer_open();

    if( gui_beat.beat_switch == 0 )
    {
        gui_info.fan_level -= 1;
        if( gui_info.fan_level <= 0 )
        {
            gui_info.fan_level = 0;
        }
        LCD_ShowNum(135,165,gui_info.fan_level,1,32,POINT_COLOR,BACK_COLOR);
        write_slave_reg();
    }

    if( key.gui_key3_allow_flag == 1 )
    {
        switch(gui_beat.beat_select)
        {
            case NTC_TEMP1_STR:
                gui_info.ntc1_temp -= 1;
                if( gui_info.ntc1_temp <= 10 )
                {
                    gui_info.ntc1_temp = 10;
                }
                break;

            case NTC_TEMP2_STR:
                gui_info.ntc2_temp -= 1;
                if( gui_info.ntc2_temp <= 10 )
                {
                    gui_info.ntc2_temp = 10;
                }
                break;

            case NTC_TEMP3_STR:
                gui_info.ntc3_temp -= 1;
                if( gui_info.ntc3_temp <= 10 )
                {
                    gui_info.ntc3_temp = 10;
                }
                break;

            case FAN_LEVEL_STR:
                gui_info.fan_level -= 1;
                if( gui_info.fan_level <= 0 )
                {
                    gui_info.fan_level = 0;
                }
                break;

            case BAKE_POWER_STR:
                gui_info.bake_power_percentage -= 5;
                if( gui_info.bake_power_percentage <= 30 )
                {
                    gui_info.bake_power_percentage = 30;
                }
                break;

            case BAKE_WIND_STR:
                gui_info.bake_wind_level -= 1;
                if( gui_info.bake_wind_level <= 0 )
                {
                    gui_info.bake_wind_level = 0;
                }
                break;

            default:
                break;
        }
    }

    buzzer_close();
    key.key_value_flag = 0;
}

void key4_press()
{
    buzzer_open();

    // if( gui_beat.beat_switch == 0 )
    // {
    //     key.sync_allow_flag = 1 - key.sync_allow_flag;
    //     sync_switch();
    // }else
    // {
        key.key4_press_cnt++; 
        if( key.key4_press_cnt == 2 )
        {
            jump_to_init();
            write_slave_reg();
        }
        if( key.gui_key4_allow_flag == 1 )
        {
            switch (gui_beat.beat_select)
            {
                case AC220_SET_ICON:
                    gui_beat.beat_select = NTC_TEMP1_STR;
                    
                    key.gui_key2_allow_flag = 1;
                    key.gui_key3_allow_flag = 1;
                    break;
    
                case AC220_SWITCH_ICON:
                    gui_info.ac220_switch = 1 - gui_info.ac220_switch;
                    ac220_switch_dis();
                    jump_to_init();
                    write_slave_reg();
                    break;
    
                case LED_ICON:
                    gui_info.led_switch = 1 - gui_info.led_switch;
                    jump_to_init();
                    write_slave_reg();
                    led_switch_dis();
                    led_crl();
                    break;
    
                case FAN_ICON:
                    gui_beat.beat_select = FAN_LEVEL_STR;
                    key.gui_key2_allow_flag = 1;
                    key.gui_key3_allow_flag = 1;
                    break;
    
                case BAKE_ICON:
                    gui_beat.beat_select = BAKE_POWER_STR;
                    key.gui_key2_allow_flag = 1;
                    key.gui_key3_allow_flag = 1;
                    break;
    
                default:
                    break;
            }
            
        }
           
    //}
    
    buzzer_close();
    key.key_value_flag = 0;
}

void buzzer_open( void )
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_SET);
}

void buzzer_close( void )
{
    delay_ms(150);
    HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_RESET);
}
