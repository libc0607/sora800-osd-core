
#include "lua.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assert.h"
#include "devman.h"
#include "platform.h"
#include "platform_conf.h"
#include "platform_fs.h"
#include "platform_rtos.h"

/*+\NEW\liweiqiang\2013.5.11\�����Խ�ѹluazipĿ¼���ļ�֧��,ѹ���㷨lzma*/
#if defined(AM_LZMA_SUPPORT)
#include "lzmalib.h"
#endif
/*-\NEW\liweiqiang\2013.5.11\�����Խ�ѹluazipĿ¼���ļ�֧��,ѹ���㷨lzma*/

#include "preload.h"

/*+\NEW\liweiqiang\2013.10.25\lua�ű�ͳһ����luaĿ¼��,Ԥ�õķ�lua�ļ�ͳһ����ldata�ļ��� */
/*+\NEW\liweqiang\2013.5.8\���ļ�ϵͳ����main.lua�ļ�ʱ����ʱ�Զ������ļ�ϵͳ�ļ�*/
#define LUA_ENTRY_FILENAME "main.lua"
#define LUA_ENTRY_FILE "/lua/" LUA_ENTRY_FILENAME
/*-\NEW\liweqiang\2013.5.8\���ļ�ϵͳ����main.lua�ļ�ʱ����ʱ�Զ������ļ�ϵͳ�ļ�*/

#define LUA_ENTRY_ENC_FILENAME "main.luae"
#define LUA_ENTRY_ENC_FILE "/lua/" LUA_ENTRY_ENC_FILENAME

/*+\NEW\liulean\2015.8.5\������߸�����MP3����������������*/
#define LUA_CHECK_INTEGRITY_FILE "/integrity.bin"
#define LUA_INTEGRITY_FLAG 0xABCD8765
/*-\NEW\liulean\2015.8.5\������߸�����MP3����������������*/

/*+\NEW\liweiqiang\2013.5.11\�����Խ�ѹluazipĿ¼���ļ�֧��,ѹ���㷨lzma*/
#define LUA_ENTRY_FILE_ZIP "/luazip/" LUA_ENTRY_FILENAME ".zip"
#define LUA_ENTRY_ENC_FILE_ZIP "/luazip/" LUA_ENTRY_ENC_FILENAME ".zip"

/*-\NEW\liweiqiang\2013.5.11\�����Խ�ѹluazipĿ¼���ļ�֧��,ѹ���㷨lzma*/
/*-\NEW\liweiqiang\2013.10.25\lua�ű�ͳһ����luaĿ¼��,Ԥ�õķ�lua�ļ�ͳһ����ldata�ļ��� */

extern char _lua_script_section_start[LUA_SCRIPT_SIZE];


/*+\NEW\rufei\2013.9.13\����lua�ļ����ܱ��ƻ����³�����������*/
void LuaDeleteMainFile(void)
{
    remove(LUA_ENTRY_FILE);
    remove(LUA_ENTRY_ENC_FILE);
}
/*-\NEW\rufei\2013.9.13\����lua�ļ����ܱ��ƻ����³�����������*/

/*+\NEW\liweiqiang\2013.10.25\lua�ű�ͳһ����luaĿ¼��,Ԥ�õķ�lua�ļ�ͳһ����ldata�ļ��� */
int file_exist(const char *name)
{
    FILE *fp;
    if((fp = fopen(name, "rb")) == NULL)
        return FALSE;

    fclose(fp);
    return TRUE;
}

/*-\NEW\liweiqiang\2013.10.25\lua�ű�ͳһ����luaĿ¼��,Ԥ�õķ�lua�ļ�ͳһ����ldata�ļ��� */

/*+\NEW\liweiqiang\2013.11.28\luadb��ʽԶ������֧�� */
static int load_luadb(void)
{
#define LUA_UPDATE_FILE "/luazip/update.bin"
    
    FILE *fp;
    int size;
    u8 *buff;
/*+\NEW\liweiqiang\2013.12.6\Ĭ�ϲ������ͷ�luadb�ļ�,����ʧ��ʱ�����ͷ�rom��luadb */
    BOOL file_override = FALSE;
/*-\NEW\liweiqiang\2013.12.6\Ĭ�ϲ������ͷ�luadb�ļ�,����ʧ��ʱ�����ͷ�rom��luadb */
    /*+\NEW\zhuth\2014.8.14\��������ɹ�ִ�������е�д�ļ�������������*/
    BOOL restart1 = FALSE;
    BOOL restart2 = FALSE;
    int result = 0;
    /*-\NEW\zhuth\2014.8.14\��������ɹ�ִ�������е�д�ļ�������������*/

    /*+\NEW\liulean\2015.8.5\������߸�����MP3����������������*/
    unsigned int integrity_flag = LUA_INTEGRITY_FLAG;
    FILE *integrity_file = NULL;
    /*-\NEW\liulean\2015.8.5\������߸�����MP3����������������*/

    if((fp = fopen(LUA_UPDATE_FILE, "rb")) == NULL)
    {
        // �ļ������� ����Ԥ����rom������
        goto load_rom_luadb;
    }
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    buff = malloc(size);
    fread(buff, 1, size, fp);
    fclose(fp);
    
/*+\NEW\liweiqiang\2013.12.6\Ĭ�ϲ������ͷ�luadb�ļ�,����ʧ��ʱ�����ͷ�rom��luadb */
    /*+\NEW\zhuth\2014.2.17\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    if(LUADB_ERR_NONE != parse_luadb_data(buff, size, TRUE, LUA_SCRIPT_TABLE_UPDATE_SECTION, &restart1))
    /*-\NEW\zhuth\2014.2.17\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    {
        removeLuaDir();
        // �Ը��Ƿ�ʽ��ѹrom��luadb,���������ܳ��ֲ������������
        file_override = TRUE;
    /*+\NEW\liweiqiang\2015.1.9\����ʧ������ļ����������� ����ĳЩ��Ŀ�ļ�������� ����֮��ϵͳ�����ڴ治�� */
        // �Զ��������������ʧ�ܵ��ļ�����
        restart1 = TRUE;
    /*-\NEW\liweiqiang\2015.1.9\����ʧ������ļ����������� ����ĳЩ��Ŀ�ļ�������� ����֮��ϵͳ�����ڴ治�� */

        /*+\NEW\liulean\2015.8.5\������߸�����MP3����������������*/
        free(buff);
        goto lua_db_script_parse;
        /*-\NEW\liulean\2015.8.5\������߸�����MP3����������������*/
    }
/*-\NEW\liweiqiang\2013.12.6\Ĭ�ϲ������ͷ�luadb�ļ�,����ʧ��ʱ�����ͷ�rom��luadb */

    free(buff);

load_rom_luadb:

    /*+\NEW\liulean\2015.8.5\������߸�����MP3����������������*/
    file_override = TRUE;

    if(file_exist(LUA_CHECK_INTEGRITY_FILE))
    {
        unsigned int integrity_context;
        integrity_file = fopen(LUA_CHECK_INTEGRITY_FILE, "rb");
        
        if(integrity_file)
        {
            fread(&integrity_context, 4, 1, integrity_file);
            
            if(integrity_context == LUA_INTEGRITY_FLAG)
            {
                file_override = FALSE;
                printf("INTEGRITY file correct!\r");
            }
            else
            {
                printf("INTEGRITY file wrong!\r");
            }

            fclose(integrity_file);
        }
        else
        {
            printf("INTEGRITY file can not open!\r");
        }
    }
    else
    {
        printf("INTEGRITY file not exist!\r");
    }
   
lua_db_script_parse:    
/*-\NEW\liulean\2015.8.5\������߸�����MP3����������������*/
    /*+\NEW\zhuth\2014.2.17\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    result = parse_luadb_data(_lua_script_section_start, LUA_SCRIPT_SIZE, file_override, LUA_SCRIPT_TABLE_FLASH_SECTION, &restart2);
    /*-\NEW\zhuth\2014.2.17\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/

    /*+\NEW\liulean\2015.8.5\������߸�����MP3����������������*/
    if(file_override == TRUE)
    {
        printf("INTEGRITY file write begin!\r");
        
        integrity_file = fopen(LUA_CHECK_INTEGRITY_FILE, "wb");
        
        if(integrity_file)
        {
            fwrite((const void*)&integrity_flag, 4, 1, integrity_file);
            fclose(integrity_file);
            printf("INTEGRITY file write success!\r");
        }
        else
        {
            printf("INTEGRITY file write failed!\r");
        }
    }
    /*-\NEW\liulean\2015.8.5\������߸�����MP3����������������*/

    /*+\NEW\zhuth\2014.8.14\��������ɹ�ִ�������е�д�ļ�������������*/
    if(restart1 || restart2)
    {
        platform_rtos_restart();
    }
    /*-\NEW\zhuth\2014.8.14\��������ɹ�ִ�������е�д�ļ�������������*/
    
    return result;
}
/*-\NEW\liweiqiang\2013.11.28\luadb��ʽԶ������֧�� */

int LuaAppTask(void)
{    
/*+\NEW\2013.7.11\liweiqiang\����luadbԤ���ļ�����*/
    int argc;
    char **argv;
    BOOL existScript = TRUE;
    BOOL existLuaDB = FALSE;
    int dbret;

    static const char *argv_null[] = {"lua", NULL};
    static const char *argv_script_const[] =
    {
        "lua",
        "-e",
        _lua_script_section_start,
        NULL
    };
/*+\NEW\liweqiang\2013.5.8\���ļ�ϵͳ����main.lua�ļ�ʱ����ʱ�Զ������ļ�ϵͳ�ļ�*/
    static const char *argv_script_file[] =
    {
        "lua",
        LUA_ENTRY_FILE,
        NULL
    };
/*-\NEW\liweqiang\2013.5.8\���ļ�ϵͳ����main.lua�ļ�ʱ����ʱ�Զ������ļ�ϵͳ�ļ�*/
    static const char *argv_enc_script_file[] =
    {
        "lua",
        LUA_ENTRY_ENC_FILE,
        NULL
    };

    if((unsigned char)_lua_script_section_start[0] == 0xff || _lua_script_section_start[0] == '\0')
    {
        argc = sizeof(argv_null)/sizeof(argv_null[0]);
        argv = (char **)argv_null;

        existScript = FALSE;
    }
    else
    {
        //����Ԥ�ýű�ʱʹ��debug����Ϊ���������
        platform_set_console_port(PLATFORM_PORT_ID_DEBUG);
    }

    //��ʼ���豸 stdio\fs\...
    if(platform_init() != PLATFORM_OK)
    {
        ASSERT(0);
    }

    dm_init();

    // ע��ƽ̨�ļ�ϵͳ�ӿ�
    dm_register(platform_fs_init());
    
    dbret = load_luadb();

    if(dbret != LUADB_ERR_NOT_DBDATA)
    {
        existLuaDB = TRUE;
    }

/*+\NEW\liweqiang\2013.5.8\���ļ�ϵͳ����main.lua�ļ�ʱ����ʱ�Զ������ļ�ϵͳ�ļ�*/
    if(existScript || existLuaDB)
    {
        BOOL exitZipFile = FALSE;
        char* zipFileName;
        char* enterFile;
    /*+\NEW\liweiqiang\2013.5.11\�����Խ�ѹluazipĿ¼���ļ�֧��,ѹ���㷨lzma*/
    #if defined(AM_LZMA_SUPPORT)
        // �����ɵ��ļ�������ʽ,�Լ��ݾɰ汾
        if(file_exist(LUA_ENTRY_FILE_ZIP) == TRUE)
        {
            exitZipFile = TRUE;
            zipFileName = LUA_ENTRY_FILE_ZIP;
            enterFile = LUA_ENTRY_FILE;
        }
        else if(file_exist(LUA_ENTRY_ENC_FILE_ZIP) == TRUE)
        {
            exitZipFile = TRUE;
            zipFileName = LUA_ENTRY_ENC_FILE_ZIP;
            enterFile = LUA_ENTRY_ENC_FILE;
        }
        if(exitZipFile)
        {
            // ֻ���ڴ����������ļ�������²Ŵ����ѹ
            int lzmaret = 0;    
            if((lzmaret = LzmaUncompressFile(zipFileName, enterFile)) == 0)
            {
                /*+\NEW\rufei\2013.9.13\����lua�ļ����ܱ��ƻ����³�����������*/
                // ��ѹ���ɹ�,ɾ��ѹ���ļ�
                /*+\NEW\zhuth\2014.8.11\��������ѹ���ɹ���ɾ������������������*/
                remove(zipFileName);
                /*-\NEW\zhuth\2014.8.11\��������ѹ���ɹ���ɾ������������������*/
                /*-\NEW\rufei\2013.9.13\����lua�ļ����ܱ��ƻ����³�����������*/
                printf("uncompress zip file success!\n", lzmaret);
                /*+\NEW\zhuth\2014.8.11\��������ѹ���ɹ���ɾ������������������*/
                platform_rtos_restart();
                /*-\NEW\zhuth\2014.8.11\��������ѹ���ɹ���ɾ������������������*/
            }
            else
            {
                printf("uncompress file error(%d)!\n", lzmaret);
            }
        }
    #endif   
    /*-\NEW\liweiqiang\2013.5.11\�����Խ�ѹluazipĿ¼���ļ�֧��,ѹ���㷨lzma*/

        if(file_exist(LUA_ENTRY_FILE) == TRUE)
        {
            printf("\r\nRUN main.lua\r\n");
            argc = sizeof(argv_script_file)/sizeof(argv_script_file[0]);
            argv = (char **)argv_script_file;
        }
        else if(file_exist(LUA_ENTRY_ENC_FILE) == TRUE)
        {
            printf("\r\nRUN ENCRYPT main.lua\r\n");
            argc = sizeof(argv_enc_script_file)/sizeof(argv_enc_script_file[0]);
            argv = (char **)argv_enc_script_file;
        }
        else if(existLuaDB)
        {
            // ����Ԥ�������޷��������ļ����޷���Ԥ�ýű�����
            printf("[lua]: luadb parse ret %d\n", dbret);
            argc = sizeof(argv_null)/sizeof(argv_null[0]);
            argv = (char **)argv_null;
        }
        else
        {
            printf("[lua]: main.lua not exist, excute from const script.\n");
            argc = sizeof(argv_script_const)/sizeof(argv_script_const[0]);
            argv = (char **)argv_script_const;
        }
    }
/*-\NEW\liweqiang\2013.5.8\���ļ�ϵͳ����main.lua�ļ�ʱ����ʱ�Զ������ļ�ϵͳ�ļ�*/
/*-\NEW\2013.7.11\liweiqiang\����luadbԤ���ļ�����*/

    return lua_main(argc, argv);
}

