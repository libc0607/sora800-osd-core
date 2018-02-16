
#include "string.h"
#include "malloc.h"

#include "rda_pal.h"
#include "assert.h"
#include "platform.h"
#include "platform_spi.h"

s32 platform_spi_setup( u32 id, u32 clock, BOOL cpol, BOOL cpha, unsigned databits, BOOL fullduplex, BOOL withCS)
{
    T_AMOPENAT_SPI_PARAM cfg;

    cfg.clock = clock;
    cfg.cpha = cpha;
    cfg.cpol = cpol;
    cfg.dataBits = databits;
    cfg.fullDuplex = fullduplex;
    cfg.withCS = withCS;
    IVTBL(send_event)(0x8b18b1);
    return IVTBL(config_spi)((E_AMOPENAT_SPI_PORT)id, &cfg);
}


s32 platform_spi_send(u32 id, u8* data, int len)
{
    return IVTBL(write_spi)((E_AMOPENAT_SPI_PORT)id, data, len);
}


s32 platform_spi_recv(u32 id, u8** data, int len)
{
    *data = malloc(len);
    
    if(*data == NULL)
    {
        return -1;
    }
    
    return IVTBL(read_spi)((E_AMOPENAT_SPI_PORT)id, *data, len);
}

s32 platform_spi_send_recv(u32 id, u8* sendData, u8** recvData, int len)
{
    *recvData = malloc(len);
    
    if(*recvData == NULL)
    {
        return -1;
    }

    return IVTBL(rw_spi)((E_AMOPENAT_SPI_PORT)id, sendData, *recvData, len);
}



