from inc_noesis import *
import noesis
import rapi


def registerNoesisTypes():
    handle = noesis.register("Sly Cooper: Thieves in Time (PS3)", ".mesh")
    noesis.setHandlerTypeCheck(handle, noepyCheckType)
    noesis.setHandlerLoadModel(handle, noepyLoadModel)
    noesis.logPopup()
    return 1


def noepyCheckType(data):
    bs = NoeBitStream(data)
    try:
        Magic = bs.readBytes(4).decode("ASCII")
    except:
        return 0
    if Magic != "MESH":
        return 0
    return 1


def noepyLoadModel(data, mdlList):
    ctx = rapi.rpgCreateContext()
    rapi.setPreviewOption("setAngOfs", "0 -60 90")  # set the default preview angle
    bs = NoeBitStream(data)
    bs.seek(0xc, NOESEEK_ABS)
    lenMSHH = bs.readUInt()
    bs.seek(lenMSHH - 4, NOESEEK_REL)
    print("1")
    meshName = 0
    check = 1
    end = len(data)
    while check < end:
        meshName += 1
        rapi.rpgSetName(str(meshName))
        SMSH = bs.readBytes(4).decode("ASCII")
        nextSMSH = bs.readUInt()
        MHDR = bs.readBytes(4).decode("ASCII")
        print("2")
        lenMHDR = bs.readUInt() - 4
        skip = bs.readByte()
        VCount = bs.readUShort()
        print(VCount, "VCount")
        FCount = bs.readUShort()
        print(FCount, "FCount")
        bs.seek(lenMHDR - 5, NOESEEK_REL)
        MVTX = bs.readBytes(4).decode("ASCII")
        Vertdata = bs.readUInt() - 4
        VBytes = Vertdata // VCount
        print(VBytes, "vbytes")
        VBuf = bs.readBytes(Vertdata)
        VBuf = rapi.swapEndianArray(VBuf, 2)
        if VBytes == 26:
            rapi.rpgBindPositionBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 0)  # position of vertices
            # rapi.rpgBindNormalBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 12)   #normals -optional
            rapi.rpgBindUV1BufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 10)  # UVs
        if VBytes == 25:
            rapi.rpgBindPositionBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 0)  # position of vertices
            # rapi.rpgBindNormalBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 12)   #normals -optional
            rapi.rpgBindUV1BufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 10)  # UVs
        if VBytes == 24:
            rapi.rpgBindPositionBufferOfs(VBuf, noesis.RPGEODATA_FLOAT, VBytes, 0)  # position of vertices
            # rapi.rpgBindNormalBufferOfs(VBuf, noesis.RPGEODATA_FLOAT, VBytes, 12)   #normals -optional
            rapi.rpgBindUV1BufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 16)  # UVs
        if VBytes == 23:
            rapi.rpgBindPositionBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 0)  # position of vertices
            # rapi.rpgBindNormalBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 12)   #normals -optional
            rapi.rpgBindUV1BufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 10)  # UVs
        if VBytes == 20:
            rapi.rpgBindPositionBufferOfs(VBuf, noesis.RPGEODATA_FLOAT, VBytes, 0)  # position of vertices
            # rapi.rpgBindNormalBufferOfs(VBuf, noesis.RPGEODATA_FLOAT, VBytes, 12)   #normals -optional
            rapi.rpgBindUV1BufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 16)  # UVs
        if VBytes == 18:
            rapi.rpgBindPositionBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 0)  # position of vertices
            # rapi.rpgBindNormalBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 12)   #normals -optional
            rapi.rpgBindUV1BufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 10)  # UVs
        if VBytes == 17:
            rapi.rpgBindPositionBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 0)  # position of vertices
            # rapi.rpgBindNormalBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 12)   #normals -optional
            rapi.rpgBindUV1BufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 10)  # UVs
        if VBytes == 14:
            rapi.rpgBindPositionBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 0)  # position of vertices
            # rapi.rpgBindNormalBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 12)   #normals -optional
            rapi.rpgBindUV1BufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 10)  # UVs
        if VBytes == 6:
            rapi.rpgBindPositionBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 0)  # position of vertices
            # rapi.rpgBindNormalBufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 12)   #normals -optional
            # rapi.rpgBindUV1BufferOfs(VBuf, noesis.RPGEODATA_HALFFLOAT, VBytes, 10)  # UVs
        MIDX = bs.readBytes(4).decode("ASCII")
        IBuf = bs.readUInt() - 4
        IBuf = bs.readBytes(IBuf)
        IBuf = rapi.swapEndianArray(IBuf, 2)
        try:
            rapi.rpgCommitTriangles(IBuf, noesis.RPGEODATA_SHORT, FCount, noesis.RPGEO_TRIANGLE,
                                    1)  # SHORT for word , INT for dword
        except RuntimeError:
            print("RuntimeError 1")
            return 0
        if bs.tell() + 4 < end:
            mpalCheck = bs.readBytes(4).decode("ASCII")
            if mpalCheck == 'MPAL':
                lenMPAL = bs.readUInt()
                bs.seek(lenMPAL - 4, NOESEEK_REL)
            elif mpalCheck == 'OUTL':
                bs.seek(end, NOESEEK_ABS)
            else:
                bs.seek(-4, NOESEEK_REL)
        check = bs.tell()
    try:
        mdl = rapi.rpgConstructModel()
    except RuntimeError:
        print("RuntimeError 2")
        return 0
    mdlList.append(mdl)
    rapi.rpgClearBufferBinds()
    return 1
