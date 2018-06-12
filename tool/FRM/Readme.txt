[Version 1.1]
增加对custom模块功能的支持

[Version 1.2]
增加RSV Only item功能的支持

[Version 1.3]
增加custom模块中对LCD的支持
增加对资源文件的保护功能--实现password

[Version 1.4]
修改在Reset后刷新资源文件显示的问题
修改在Reset后customize首次不能正常工作的问题

[Version 2.0]
增加了对FSG文件加密解密的功能
与FSG进行了联合测试，验证了加密解密功能
进行了系统测试，解决了很多隐藏的问题。

[Version 2.1]
在用户set working folder时展开当前working path
增加用户program All时对working folder下A，B目录文件总大小的判断
对FSG产生文件read verify时进行先解密再比较，解决FSG产生文件read verify不成功的问题。

[Version 2.2]
解决直接RSV Only可能不成功的问题。
添加tooltip来解决显示路径不完整的问题。
解决1 byte upload不成功的问题。
增加快捷键：ISP：F5  USB：F6。

[Version 2.3]
解决tooltip中切换working folder后存在显示混乱的问题。
解决0/1 byte的文件read back会挂起的问题。
解决大firmware不能正常upload的问题。

[Version2.4]
增加对配置文件（FRM_Setting.ini, Customize.ini, USB_CAM5000.ini）在working floder
    下的读取功能，FRM会首先读取working floder下的配置文件来进行配置，若无，则读取
    默认的配置文件来进行配置。isp.bin的读取方式同配置文件。
    
[Version2.4.1]
修改在切换working folder之后F/W, RawC路径显式的Tooltip没有及时更新的问题。
修改因为回写配置文件导致ulCoreAddr地址为空而FRM不能正常运行的问题。

[Version2.4.2]
修正FRM 在xp系统下会always on top的bug。
修正FRM在USB1.1下因延时问题引起F/W烧录出现time out的bug。

[Version2.4.3]
将查询fw返回的做BulkIn 和BulkOut的size在传输之前与64k比较，以较小的size作bulk传输，
    重新解决USB1.1下烧录延时引起的问题。
    
[Version2.4.4]
进行了一轮系统测试，解决了FRM中还存在问题。
解决删除working folder下选中文件夹中存在子文件夹会出现异常的问题。
解决F/W +Partial RSV中有FSG文件时回读比较不一致的问题。
解决对working folder下配置文件回写会创建新文件而导致下次启动功能执行不正常的问题。

[Version2.4.5]
修改回归测试中的问题。
修改reset后直接删除资源文件，第一次不能正常删除的问题。

[Version2.5.0]
增加Card ISP的功能。

[Version2.5.1]
修改在Program All时对分区信息的传递。
修改FRM_setting.ini中default的分区信息。

[Version2.5.2]
isp.bin版本升级到0.64.
修改MFC程序属性中关于文件和产品的版本.

[Version2.5.3]
add a memory clear code before read back firmware from nand 
     to solve the program all read verify comparing error problems.
     
[Version2.5.4]
1.在FrmDirEx()中对change directory的状态判断不全面导致两次change dirctory。
2.解决Get单个文件使没有创建对应文件夹导致的错误。
3.在要覆盖文件时先删除vfs以存在文件再进行创建来协助解决vfs因覆盖引起的问题。

[Version2.5.5]
1.Program All或SetSize时根据Working folder下文件的大小估算Disk分区Size；
2.平台分区Size与Ini文件中默认值同步更新；
3.解决SetSize时，Size错误引起程序不能退出的Bug。

[Version2.6.0]
1．增加生成Disk Image和Download Disk Image的功能；
2．在Card ISP中，转换小写目录名为大写；
3．将“Create Card ISP”menu移到Working Tree上；
4．修改Card ISP Packet时应首先选择Working Folder下isp.bin。

[Version2.6.0.1]
1．增加Nand Test的功能；
2．增加根据Working Folder生成Disk Image的功能；
3．Query功能数据Size小于等于(256-12)时不用Bulk，原来是小于(256-12)不用Bulk；
4．此版本未经QA测试，仅供内部测试Nand Test功能用。

[Version2.6.0.2] [isp.bin : V0.68] : 2006/09/29
1．Nand Test功能不可用(isp.bin未完成，测试时设置TestFlg＝1);
2．解决在Card ISP中，转换小写目录名为大写时将文件名改为大写的Bug；
3．解决Get Image时，若Size比原有Image的小，覆盖后Size不对的Bug；
4．解决isp.bin burn disk image死机的Bug；
5．解决RSV覆盖download时，有新增文件且选择不覆盖已有文件时的Bug；
6．新增从Working Folder下Create disk image的功能(不支持带空格的working folder，不支持加密文件，FAT12最大Disk Size为65344K)。

[Version2.6.0.3] [isp.bin : ?, DLL : 2.6.0.2] : 2006/10/03
1．在Create Disk Image不成功时删除临时文件;
2．当Working Folder下没有FRM_Setting.ini时才从FW读RSV info;
3．在Get RSV Info界面增加估算分区Size的功能;
4．此版本给Nand Test测试ISP Core用，Disk Image功能不可用。

[Version2.6.0.4] [isp.bin : ?, DLL : 2.6.0.3] : 2006/10/04
1．增加"Erase Later Defect Blk"功能;
2．增加Vendor Command的Timeout时间VendorCommandBulkRead为45秒(Nand Test), VendorCommandBulkWrite为40秒，VendorCommandBulkWrite64为40秒； 
3．此版本给Nand Test测试ISP Core用，Disk Image功能不可用。

[Version2.6.0.5] [isp.bin : V0000.0069, DLL : 2.6.0.3] : 2006/10/11
1．增加在AP标题栏显示FRM和ISP版本功能(启动和USB连接时更新ISP版本，根据资源文件中的版本号显示FRM版本);
2．解决Create Disk Image不能回读和删除长档名文件的问题;
3．删除文件时根据文件数目延时;
4．增加Turbo Burn后的verify Disk Image功能。

[Version2.6.0.6] [isp.bin : V0000.0070, DLL : 2.6.0.3] : 2006/10/12
1．解决isp v0.66版本之后Card ISP失败的问题;
2．将Turbo Mode选择改到Advanced界面。 

[Version2.6.1] [isp.bin : V0000.0070, DLL : 2.6.1] : 2006/10/17
1．改正menu拼写错误和menu快捷键;
2．A，B的Count Size增加reserved swap block，与RSV Info统一;
3．Create RSV Image不支持带空格的working folder path, 增加提示信息并操作无效;
4．增加对ESCAPE键的处理避免Dialog变灰；
5．改正RSV Only + Verify时没有删除Temp目录的问题；
6．改正memory leak；
7．改正多个同名文件覆盖并存在不同文件时没有完全删除平台同名文件的问题(vfs的限制)。
8．去掉Set Working Folder后的custom命令，不连接平台时不发送custom命令;
9．不连接平台时在FRM版本后不显示‘－’。

[Version2.6.2] [isp.bin : V0000.0071, DLL : 2.6.2] : 2006/10/30
1. 优化RSV Image生成功能，不再使用imgtl.exe；
2. 优化Card ISP运行速度，改变ISP mode setting窗口，增加Burn file pathname setting；
3. 修正Turbo Mode, Bad black upload和Bad black test选项在Set working folder后失效；
4. 增加Log Window，在程序长时间无响应时给用户提示信息；
5. 修改界面，将Working Folder, RawC Pahtname和FW Pathname转移到Main Window上；

[Version2.6.3] [isp.bin : V0000.0071, DLL : 2.6.2] : 2007/01/18
1.增加在Rsv info中 set size功能对read verify的引用。
2.在Rsv Get时，增加可以让用户自定义存放路径的对话框。

[Version2.6.4] [isp.bin : V0000.0086, DLL : 2.6.4] : 2007/03/06
1.DLL增加error handling 接口；增加FRM_diskImg.cpp;
2.增加error handling，增加Nand Type Select；
3.去掉swap block, RsvSize不包括swap block, 判断RsvSize；
4.改正Card ISP中计算RawC offset的Bug(应计算Rsv Header Info Size)；
5.改正Card ISP中更新文件的Bug(先删除)；
6.isp中未更新error handling相关代码;
7.增加Create Card ISP时对A，B Size的判断；
8.在Customize.ini中增加Nand Type的步骤：
    1.在[NAND_TYPE]中添加自定义的名称XXXX，如：
        [NAND_TYPE]
        SAMSUNG_SLC_128M
        SAMSUNG_MLC_1G    
        XXXX
    2.增加以自定义的名称XXXX命名的Section和Nand描述信息，如：
        [XXXX]
        DevId = 0xa1
        BlockBytes = 0x20000
        MegaBytes = 512

[Version2.6.5] [isp.bin : V0000.0087, DLL : 2.6.5] : 2007/03/12
1.在FrmNandInfoHeaderGet()中增加nandDetect()以解决有时get dev ID为0的问题；
2.在Nand Driver中增加Get nand devInfo的接口,此接口只提供给ISP，不影响其他功能;
3.在Frm_Setting.ini中增加[ISP_WORK]以兼容FRM Mass Tool。

[Version2.6.6] [isp.bin : V0000.0088, DLL : 2.6.5] : 2007/03/14
1.在Customize.ini中增加Nand描述信息，定义4字节的DevId，增加DevIdMask，如：
	[THOMSON_SLC_128M]
	DevIdMask = 0x00ff0000      //for DevId=xxf1xxxx
	DevId = 0x20f18015
	BlockBytes = 0x20000        //0x20000: 128K
	MegaBytes = 128
2.在Rsv Info对话框显示已知Nane名称或未定义的Nand的Id；
3.修改ISP Code以获得4字节的DevId。

[Version2.6.7] [isp.bin : V0000.0088, DLL : 2.6.5] : 2007/03/19
1.在Polling Interrupt pipe时增加Sleep以降低cpu占用率
2.在Frm_Setting.ini中回复Rsv_Swap_Blk和SectBlkDefSize以保持兼容以前版本

[Version2.6.8] [isp.bin : V0000.0089, DLL : 2.6.5] : 2007/03/20
1.修改isp.bin中nand driver计算超过1024个block时的logblk的bug。

[Version2.6.9] [isp.bin : V0000.0090, DLL : 2.6.5] : 2007/03/23
1.在isp.bin中Card isp流程开始前加NandDetect；
2.修改isp.bin中card isp在删除不存在的文件时读取文件属性的Bug；
3.修改单选文件时显示文件名变成问号的Bug。

[Version2.6.9.1] [isp.bin : V0000.0091, DLL : 2.6.5.1] : 2007/04/19
0.内部测试版本
1.在card_isp.cpp中订正判断文件名长度的Bug；
2.card isp增加支持Disk Image的功能；
3.isp.bin订正cardIspInfoBuf被破坏的Bug；
4.DLL增加Platform ID, Add Command for SPCA5110

[Version2.6.9.2] [isp.bin : V0000.0091, DLL : 2.6.5.1] : 2007/04/20
0.QA测?园姹?
1.增加ShowDifference功能

[Version2.6.9.3] [isp.bin : V0000.0091(fw:2007/05/11), DLL : 2.6.5.2] : 2007/05/14
0.内部测试用版本
1.FRM修改Read Verify Rsv File时显示信息；
2.更换平台时，如果Customize.ini中的isp文件不存在，应选择默认isp文件，而不是保留前一个平台的isp文件；
3.启动FRM时不连接平台；
4.只在Turbo模式检查Flash Device ID；
5.连接平台时增加对SPCA5110 PID的判断；
6.屏蔽ShowDifference功能。

[Version2.6.9.4] [isp.bin : V0000.0091(fw:2007/05/11), DLL : 2.6.5.2, ShowDiff : 0.0.1.1] : 2007/05/22
0.QA回归测试
1.修改ShowDiff中对CreateInfoList初始化cmpResult错误和浅拷贝造成两次释放资源的问题;
2.屏蔽隐藏文件和目录。

[Version2.7.0] [isp.bin : V0000.0091(fw:2007/05/11), DLL : 2.7.0.0, ShowDiff : 0.0.2] : 2007/06/04
1. 修改FRM tooltip的显示。
2. 解决DiffShow只有单个文件显示不同时会down以及空文件夹比较出现程序异常的问题。
3. 解决FAT和NTFS的区别造成DiffShow显示出错的问题。
4. 解决Card ISP时，Image File不存在造成Disk Size为0的问题。
5. 解决Read Verify时，未指定RawC时，也会读RawC的问题。
6. DLL只改版本号。

[Version2.7.0.1] [isp.bin : V0000.0091(fw:2007/05/11), DLL : 2.7.0.0, ShowDiff : 0.0.2] : 2007/06/13
0.提供给NS测试，解决disk size超过1938个Block
1.FRM Create Image时根据FRM_Setting.ini中选择cluster size
2.屏蔽对话框显示Flash Id
3.修改cluster size的方法：
    1.修改FRM_Setting.ini中ClustSize0～2，改变Create Image时，Disk的Cluster Size；用于Disk Image方式。
    2.普通方式的Program All（非Turbo模式）下，修改FRM_Setting.ini中的Cluster Size外，还需要修改libvfs中Cluster Size并重新Build isp.bin。

[Version2.7.0.2] [isp.bin : V0000.0091(fw:2007/05/11), DLL : 2.7.0.0, ShowDiff : 0.0.2] : 2007/06/13
0.提供给NS测试；
1.Program All + Read verify时，去掉隐藏目录文件的比较；
2.Create Image时，如果隐藏目录下有非隐藏文件，会失败，已改正；

[Version2.7.0.3] [isp.bin : V0000.0093, DLL : 2.7.0.3, ShowDiff : 0.0.2] : 2007/07/12
0.提供给QA测试；
1.Burn Select时，从平台上判断file是否存在(FRM, DLL, ISP)；
2.Fix bug when find usb device from register table(DLL), please define "DriverName" in [BULKGUID] of USB_CAM5000.ini if bulk driver is not "Bulk5000"
3.Fix bug of refreshing working folder tree when get rsv root to a directory under the working folder(FRM);
4.Fix ddram's bug of isp;
5.Fix isp's bug when find bad block in firmware;

[Version2.7.1] [isp.bin : V0000.0093, DLL : 2.7.0.4, spca5110isp.bin : V0.76] : 2007/07/17
1.packet spca5110isp.bin

[Version2.7.1.3] [isp.bin : V0000.0095, DLL : 2.7.0.5, spca5110isp.bin : V0.79] : 2007/07/31
0.提供给QA测试；
1. Add dram type select for both SPCA5000 & SPCA5110
2. After set working folder, set the bin file as firmware if there is only one bin file.
3. The brn file's default name depend on platform's type.
4. Fix bug in card isp of disk image mode.
5. Add null directory(no files) in card isp.

[Version2.7.2] [isp.bin : V0000.0096, DLL : 2.7.0.5, spca5110isp.bin : V0.80] : 2007/08/13
isp.bin(0.96):
 1. Fix isp's card isp bug of counting RawC size without firmware;
 2. Fix isp's bug when burning an exitsted rsv file which the filename cannot recognized after turbo mode;
spca5110isp.bin(0.79):
 1. Add null directory(no files) in card isp;
 2. Modify isp boot mode;
 3. Fix isp's card isp bug of counting RawC size without firmware;(5110 not support RawC)
 4. Add dram parameter's setting;
 5. Add detecting A and B after burning disk image.
spca5110isp.bin(0.80):
 1. Fix bug of connecting with rom boot. 
FRM.exe(2.7.2):
 1. Enable adding new dram type. 

[Version2.7.2.1] [isp.bin : V0000.0096, DLL : 2.7.0.7, spca5110isp.bin : V0.80] : 2007/09/01
 0.QA回归测试；
FRM.exe(2.7.2.1)
 1.增加取消RawC的功能；
 2.Create card ISP，保持选择brn文件路径时显示的文件名和默认的文件名一致；
 3.限制brn文件的扩展名。 
USB_CAM.DLL(2.7.0.7)
 1.Add special vendor command for spca5110

[Version2.7.2.2] [isp.bin : V0000.0096, DLL : 2.7.0.7??, spca5110isp.bin : V0.80??] : 2007/09/01
 0.Test version for SN testing Nand；
FRM.exe(2.7.2.2)
 1.Add "nandPhyPageRead Test" menu
spca5110isp.bin: V0.80??
 1.Add nandPhyPageRead 

[Version2.7.2.3] [isp.bin : V0000.0096, DLL : 2.7.0.8, spca5110isp.bin : V0.80??] : 2007/09/21
FRM.exe(2.7.2.3)
 1.Checking NAND FLASH ID dont effect isp flow
 2.Define brn file path
 3.Save new flash type
USB_CAM.DLL(2.7.0.8)
 1.Fix memory leak bug

[version2.7.3.0] [isp.bin: V0000.0096, DLL:2.7.0.8, spca5110isp.bin: V0.81]: 2007/11/06
1. remove password requiring when getting res files.
2. add FSG files decrypt solely.
spca5110isp.bin(V0.81)
 1.Add nand badblk test
 2.move card isp mark to register

[version2.7.4] [isp.bin: V0000.0096, DLL:2.7.0.8, spca5110isp.bin: V0.84]: 2007/11/13
FRM.exe(2.7.4.0)
 1. Enable define special isp.bin file for card isp only(Not Recommended):
    a. Define "CardISPName=xxxx" in Frm_Setting.ini
    b. Recommend saving Frm_Setting.ini at the working folder.
    c. Save isp file "xxxx" at the same directory of Frm_Setting.ini
 2. Order of checking isp : 
    a. Frm_Setting.ini in Workingfolder(named by Platform -> named in Frm_Setting.ini)
    b. Executable path (named by Platform -> named in Frm_Setting.ini) 
spca5110isp.bin(V0.84)
 1. Build 2007/11/12

[version2.7.5] [isp.bin: 5050 V0000.0100, DLL:2.7.0.8, spca5110isp.bin: V0.84]: 2007/11/20
FRM.exe(2.7.5.0)
 1. Just update version for packeting isp.bin of 5050 v0.100 for PMP customer
isp.bin(5050 V0.100)
 1. Fix bug on 128M and 512M flashes.

[version2.7.6] [isp.bin: V0000.0096, DLL:2.7.0.8, spca5110isp.bin: V0.86]: 2007/11/22
FRM.exe(2.7.6)
 1. Add flag in isp.bin file when create card isp.
isp.bin(0.96)
 1. Packet isp.bin of 5000 v0.96
spca5110isp.bin(0.86)
 1. Set or clean card isp flag by the mark in isp.bin file. 
Note!!!: The spca5110isp.bin(0.86) cannot be used to create card isp with the old version of FRM.

[version2.7.7] [isp.bin: V0000.0096, DLL:2.7.7, spca5110isp.bin: V0.86]: 2007/11/26
FRM.exe(2.7.7)
 1. Modify dram parameters of isp and firmware files when change dram type
USB_CAM.DLL(2.7.7)
 1. Remove changing isp and firmware files when do isp connection

[version2.8.0.1] [isp.bin: V0000.0096, DLL:2.7.7.2, spca5110isp.bin: V0.86]: 2007/12/17
 0. test version to QA；
FRM.exe(2.8.0.1)
 1. Replace Customize.ini and FRM_Setting.ini with frm_base.ini, xxx_profile.ini, frm_user.ini
    a. frm_base.ini : define common information and independent to platform.
    b. \profile\xxx_profile.ini : describe platform information.
    c. frm_base.ini : describe the user's selections. FRM ceate this file at the working folder when specify the working folder.
    d. user_profile.ini : optional, which under the working folder is used to define the customer platform's information.
 2. Move "platform", "Dram Boot Parameter" and "flash type" selections to "Advanced->Set Working Folder"
 3. Platform Name is shown at the middle top of the "Basic" page.
 4. Flash Type(NAND, NOR, SPI) is shown at the letf bottom corner of the "Basic" page.
 5. The filenames of setting file and dram boot parameter file are shown.
 6. Enable selecting setting file and dram boot parameter file at the RSV tree.
 7. The user can select platform, select isp file, define dram boot parameter, lock the Basic options when set working folder.
 8. The user can create his own user_profile.ini when set working folder.  
USB_CAM.DLL(2.7.7.1)
 1. Mask special vendor command for SPCA5110 custuomer.
USB_CAM.DLL(2.7.7.1)
 1. Add the flow of downloading parameter file before isp for 1528.

[version2.8.0.2] [isp.bin: V0000.0096, DLL:2.7.7.2, spca5110isp.bin: V0.86, spca5050isp.bin: V0.10?]: 2008/01/09
 0. test version to QA;
FRM.exe(2.8.0.2)
 1. Fix bug.
 2. Change dialogs to menus.
 3. Delete user_profile.ini and add dialog to add new profile.

[version2.8.0.3] [isp.bin: V0000.0097, DLL:2.7.7.2, spca5110isp.bin: V0.86, spca5050isp.bin: V0.10?]: 2008/01/16
 0. test version to QA;
FRM.exe(2.8.0.3)
 1. Fix bug of decrypting FSG file.
isp.bin(0.97)
 1. Mask printf which length over buffer size.

[version2.8.1] [isp.bin: V0000.0097, DLL:2.7.7.2, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101]: 2008/02/15
FRM.exe(2.8.1)
 1. Support filelist when card isp. Select "Create Rsv Filelist" menu from 'Advanced->Working Folder' and a file of 'filelist.ini' is created at the working folder.

[version2.8.2] [isp.bin: V0000.0097, DLL:2.7.7.2, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101]: 2008/02/16
FRM.exe(2.8.2)
 1. Fix bug when set working folder.

[version2.8.3] [isp.bin: V0000.0097, DLL:2.7.7.3, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101]: 2008/03/10
FRM.exe(2.8.3)
 1. Add filelists for card isp and backup.
 2. Backup and restore files when program all in normal mode and turbo mode.
DLL(2.7.7.3)
 2. Support 2800.

[version2.8.3.1] [isp.bin: V0000.0097, DLL:2.8.3.1, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101]: 2008/03/27
temporary version for spmf2800 serial flash, not tested by QA
FRM.exe(2.8.3.1)
 1. Add selection of encrypt or not when creating disk image.
 2. Enable define file system info in xxx_profile.ini.
 3. Enable define AlignSize in frm_user.ini.
 4. Enable specify path of creating disk image. 
DLL(2.8.3.1)
 1. Add selection of encrypt or not when creating disk image.
frm_user.ini(1.1):
 1. Eanble define AlignSize
spca5000_profile.ini(1.1):
spca5110_profile.ini(1.1):
 1. Add "EncImageFunc = 1" and "AlignSize = 0x20000" for compatibility.
spca5050_profile.ini(1.1):
 1. Add "EncImageFunc = 1" and "AlignSize = 0x40000" for compatibility.

[version2.8.3.2] [isp.bin: V0000.0097, DLL:2.8.3.1, spca5110isp.bin: V0.87?, spca5050isp.bin: V0.101]: 2008/03/27
FRM.exe(2.8.3.2)
 1. Back up to another directory.
 2. Write Nand pages.
spca5110isp.bin(0.87)
 1. Write Nand pages

[version2.8.3.3] [isp.bin: V0000.0097, DLL:2.8.3.1, spca5110isp.bin: V0.87?, spca5050isp.bin: V0.101]: 2008/04/02
 0. test version to QA;
FRM.exe(2.8.3.3)
 1. Enable to copy files to working folder when first set this working folder
 2. Add msg when backup
 3. Enable to restore the selection of parameter file after cancel it 

[version2.8.3.4] [isp.bin: V0000.0097, DLL:2.8.3.1, spca5110isp.bin: V0.87?, spca5050isp.bin: V0.101]: 2008/04/09
 0. test version to QA;
FRM.exe(2.8.3.4)
 1.fix bug of CloseHandle
 2.set 100% after backup

[version2.8.3.5] [isp.bin: V0000.0097, DLL:2.8.3.1, spca5110isp.bin: V0.87?, spca5050isp.bin: V0.101]: 2008/04/11
FRM.exe(2.8.3.5)
 1.Enable clustSz = 1 when create disk image

[version2.8.3.6] [isp.bin: V0000.0097, DLL:2.8.3.1, spca5110isp.bin: V0.87?, spca5050isp.bin: V0.101]: 2008/04/11
 0. Requested by 2800
FRM.exe(2.8.3.6)
 1.Do not care flash block size when Program All if PlatformId is 2800

[version2.8.3.7] [isp.bin: V0000.0097, DLL:2.8.3.1, spca5110isp.bin: V0.87?, spca5050isp.bin: V0.101]: 2008/04/21
 0. Fix bugs of QA testing.
FRM.exe(2.8.3.7)
  1.Disable to select isp, dramboot and parameter files as partial files when "Select All"
  2.Disable to copy (drag drop) files when burning
  3.Fix bug when backup read verify
  4.Retrun error and stop operating when fail in backup process
  5.Fix bug of doing "Enable dramboot"
  6.Disable to select mult-files when "Decrypt Single File" 

[version2.8.4] [DLL:2.8.3.1]: 2008/04/24
 0. Release from FRM2.8.3.7 
 [isp.bin: V0000.0097, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101, spca5210isp.bin: V0.85]
FRM.exe(2.8.4)
  1.Add files for spca5210

[version2.8.5] [DLL:2.8.3.2]: 2008/04/29
 0. Fix bug and release 
 [isp.bin: V0000.0097, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101, spca5210isp.bin: V0.85]
USB_CAM.DLL(2.8.3.2)
  1.Download parameter file only when mask boot(bootmethod=0).(ISP bootmethod=3, FWbootmethod=1 or 2).
FRM.exe(2.8.5)
  2.No change.
  
[version2.8.5.1] [DLL:2.8.3.3]: 2008/06/19
 [isp.bin: V0000.0097, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101, spca5210isp.bin: V0.85]
USB_CAM.DLL(2.8.3.3)
  1.Add dram boot vendor command for 5110.
FRM.exe(2.8.5.1)
  2.Allow allocate size differe not bigger the one nand block size for 2800

[version2.8.5.1] [DLL:2.8.3.2]: 2008/07/03
 [isp.bin: V0000.0097, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101, spca5210isp.bin: V0.85]
USB_CAM.DLL(2.8.3.2)
  1.Recover 2.8.3.2 because the modification on 2.8.3.3 is invalid.

[version2.8.5.2] [DLL:2.8.3.2]: 2008/07/09
 [isp.bin: V0000.0097, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101, spca5210isp.bin: V0.85]
FRM.exe(2.8.5.2)
  1.Add voice prompt when ISP finished or error.
  2.Add switches of dialog and voice prompt. 

[version2.8.5.3] [DLL:2.8.3.2]: 2008/07/15
 [isp.bin: V0000.0097, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101, spca5210isp.bin: V0.85]
FRM.exe(2.8.5.3)
  1.Update fw and isp file by dram boot parameter only when "DramBootParFunc" is setting in the profile.

[version2.8.5.4] [DLL:2.8.3.2]: 2008/09/22  
 [isp.bin: V0000.0097, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101, spca5210isp.bin: V0.85]
FRM.exe(2.8.5.4)
  1.Modify isp or firmware file (0x100) by flag of "Enable Dram Boot Param".

[version2.8.5.5] [DLL:2.8.3.3]: 2009/02/27  
 [isp.bin: V0000.0097, spca5110isp.bin: V0.86, spca5050isp.bin: V0.101, spca5210isp.bin: V0.85]
FRM.exe(2.8.5.5)
  1.Add menu for checking factory bad block num before Program All.
USB_CAM.DLL(2.8.3.3)
  1.Add vendor command for check factory bad block num(0x79,0x0e,0x01,0x14)
frm_user.ini(1.3)
  1.Add CheckFactoryBadBlkFlag and MaxBadBlkNum settings.  

[version2.8.5.6] [DLL: 2.8.3.4]: 2009/04/07
1. Add encrypt/decrypt judgement for burning and comparing files( for FSG files especially).

[version2.8.5.7]: 2009/04/22
1. Add MP Mode in FRM. ( if use MP Mode, should release working folder under FRM Directory.)
2. In Mp Mode, it will create disk image silently if there lacks disk image in "Program All".

[version2.8.5.8] [DLL: 2.8.3.5]: 2009/06/2
1. Add Simplify Mode in FRM.( working for 28xx, do not format flash and write BIMG( on serial Flash) in turbo mode ).
2. separate the FrmFlashFormat into 2 methods: Format A,B and format User disk in DLL.

[version2.8.5.10] [DLL: 2.8.3.5]: 2009/07/09
FRM.exe(2.8.5.10)
  1. Add check sum in BRN
  2. Remove the limitation of BRN filename's suffix.

[version2.8.5.11] [DLL:2.8.3.5]: 2010/01/13
1. move the temp file in creating disk image from disk C to executing directory. 

[version2.8.5.12] [DLL:2.8.3.5]: 2010/01/25
1. replace the disk image maker with the ResImageMaker v0.0.6.

[version2.8.5.13] [DLL:2.8.3.6]: 2010/03/05
1. support 5310 SPI.

[version2.8.5.14] [DLL:2.8.3.6]: 2010/03/29
1. RsvImageMaker support mpMode != 1

[version2.8.5.15] [DLL:2.8.3.6]: 2010/06/01
1. add backup filelist for CardUpdate

[version2.8.5.16] [DLL:2.8.3.6]: 2010/06/17
1. add rawC pack for "Program All Disk Image".

[version2.8.5.17] [DLL:2.8.3.6]: 2010/11/26
1. fix the timeout bug in nand badblock test & upload. You can specify the timeoutInSeconds item in frm_base.ini( in second uint ).

[version2.5.8.18] [DLL:2.8.3.6]: 2010/12/16
1. delete the read verify folder directly, not put it in recycle bin.
2. if A, B not used, then alloc size 0 for them.( remove the auto generate A, B function ,and not download PSW file in A )

[version2.5.8.20] [DLL:2.8.3.6]: 2010/12/16
1. fixed the bug in 2.8.5.18
2. add DotNetFramework detect for create diskImage.

[version2.5.8.21] [DLL:2.8.3.6]: 2011/04/02
1.Use extern tool to create ProgramAll disk image.
2.Check .net framework 2.0 before FRM init.

[version2.8.5.27]
1.Add "query" interface for image size.

[version.2.8.5.28]
1.Fix the card ISP pack error if flash type is eMMC.
2.Fix the card ISP align size error.

[version.2.8.5.29]
1.Fix the total block size calc error.

[version.2.8.5.30]
1.Add dram parameter download in advanced page.
2.Fix the size calc error when pack ISP file in CardISP function.

[version.2.8.5.32]
1.Pack dram parameter file at the end of the CardISP file.

[version.2.8.5.33]
1.Modify the dram parameter download addr for normal ISP.
2.Add normal ISP dram parameter addr to profile setting.

[version.2.8.5.34]
1.Add a new function to get SPI flash image.

[version.2.8.5.35]
1.Change name(remove SPI prefix).

[version.2.8.5.36]
1.Add check isp core and check flash type two steps before get falsh iamge.

[version.2.8.5.38]
1.remove unused CardISP mode.

[version2.8.5.39]
1.do time sync from PC if "TimeSync" option is enable in spca6330_profile.ini.

[version.28.5.40] [DLL:2.8.4.1]: 2013/05/21
1. download dram parameter again in isp mode for V35.
2. add NAND_ADV support.

[version2.8.5.41]
1. add an option for packing dram parameter or not.

[version2.8.5.42] [DLL:2.8.5.0]: 2013/07/30
1. move all FrmStorageTypeSelect() into FrmCheckIspCon()

[version2.8.5.43] [DLL:2.8.5.1]: 2014/02/10
1. Enable filesize of BIMG is smaller than its located size for linux ipcam project.
2. Note: AIMG size should equal to its located size because of ISP.

[version2.8.5.44] [DLL:2.8.5.1]: 2014/02/28
1. Add msg of Read Verify Error.

[version2.8.5.45] [DLL:2.8.5.1]: 2014/03/20
1. Fix the error(FRM2.8.5.43) of which IMG size is not equal to the default size set in the frm_user.ini.
2. Note: Please set "IgnoreIMGSize=1" in the [CURRENT] section of frm_user.ini, if your BIMG file size is smaller than the allocated size.

[version2.8.5.46] [DLL:2.8.5.2]: 2014/04/11
1. DLL: No need download DRAM Param when ISP boot mode.

[version2.8.5.47] [DLL:2.8.5.3]: 2014/05/05
1. DLL: Add Frm_ISPNandBadBlkTotalGet() and related vendor command()
2. FRM: Add menu of "Check Max Bad Blk" in Advanced page.
3. Note: Program All will has no response when enable "Check Max Bad Blk" without ISP's support.

[version2.8.5.48] [DLL:2.8.5.3]: 2014/09/05
1. FRM: Support net path for ipcam.

[version2.8.5.49] [DLL:2.8.5.4]: 2014/09/18
1. FRM: Resend vendor command if ERROR_SEM_TIMEOUT when use usb3.0 port.

[version2.8.5.50] [DLL:2.8.5.5]: 2014/10/14
1. DLL: Resend vendor command if ERROR_GEN_FAILURE when use usb3.0 port.

[version2.8.5.51] [DLL:2.8.5.5]: 2014/12/03
1. keep comboBox and status show the same storage type
2. save the storage type selection in frm_user.ini
3. add "StorageTypeSelFunc" in profile to enable or disable the comboBox of storage type

[version2.8.5.52] [DLL:2.8.5.5]: 2015/01/26
1. Add GPIO_GRP_FML and GPIO_GRP_FMH
2. Add Param read verify
3. Insert "GPIO_GRP_FML" and "GPIO_GRP_FMH" in [CUSTOM_GPIO_GROUP] before GPIO_GRP_GEN in "frm_base.ini" file, and define [GPIO_GRP_FML]  and [GPIO_GRP_FMH] at the same time, if you want use FMGPIO

[version2.8.5.53] [DLL:2.8.5.6]: 2015/02/05
1. Add "UsbReConWaitMs" in [CURRENT] section of frm_user.ini, to enable configure USB Re-connection wait time. default 7000ms.

[version2.8.5.54] [DLL:2.8.5.7]: 2015/02/10
1. Fix "Create Card ISP" fail problem because of C root directory doesn't have write permission. When execute "Create Card ISP", create FRMTEMP file in the directory with the same level of "frm.exe". 

[version2.8.5.55] [DLL:2.8.5.8]: 2015/03/04
1. Add flag "EncSstSfnFunc" in "FUNC_LIST" in profile ini file, to enable or disable encrypt sst/sfn file. Default enable. 

[version2.8.5.56] [DLL:2.8.5.8]: 2015/03/06
1. Default disable encrypt sst/sfn.

[version2.8.5.57] [DLL:2.8.5.9]: 2015/05/13
1. Add flag "FastbootFlagClr" in "CURRENT" in "frm_user.ini" file to send command to clean RTC related register for IPCam.

[version2.8.5.58][DLL:2.8.5.9]: 2015/07/29
1. Check the difference between resource folder and IMG. Remind if the resource folder changed. Add "CmpImgRes=0" in "CURRENT" of "frm_user.ini" file to disable this check. 
2. Add menu to show img log files' diff with extra compare tool of UltraEdit's ucl.exe .

[version2.8.5.59][DLL:2.8.5.9]: 2015/08/24
1. Add error msg for Param readverify.

[version2.8.5.60][DLL:2.8.5.10]: 2015/12/24
1. DLL : Check param file when usb connection
2. DLL : Add Vendor Command to check the status of DRAM Param
3. FRM : Add msg for checking param file

[version2.8.5.61][DLL:2.8.5.11]: 2016/05/16
1. Support FW2 for V50. Should enable this function in profile, add "FW2Func=1" in [FUNC_LIST]
2. Support FW/FW2 pre-allocation for V50.
3. Support stroage tpyes of "SD" and "eSD". Should add "SD" or "eSD" in frm_base.ini if necessary.
4. Change company logo and icon.
5. Support FW2 and pre-allocation for V50 card isp.

[version2.8.5.62][DLL:2.8.5.12]: 2016/05/17
1. Change the versions and rebuild because the version2.8.5.61 is used for testing.

[version2.8.6.0][DLL:2.8.5.12][ResImageMaker:1.0.3.0]: 2016/08/02
1. Support trade maker for BRN file. Add "TradeMakerFunc" in [FUNC_LIST] and "TradeMaker" in [Profile] of profile to enable and define trade maker. Should confirm the trade maker is supported by the firmware first.
2. Modify the manufacture name of IMG.

[version2.8.6.1][DLL:2.8.5.12][ResImageMaker:1.0.3.0]: 2016/09/02
1. Select burning the existed IMG or Select creating new IMG and burning when resource file changed.

[version2.8.6.2][DLL:2.8.5.12][ResImageMaker:2.0.0.0]: 2016/09/29
1. Support .Net4.0 .
2. Check .Net version

[version2.8.6.3][DLL:2.8.5.12][ResImageMaker:2.0.1.0]: 2016/10/25
1. Fix fat size error
2. Support setting [PLATFORM_GPIO_GROUP] in profile if GpioGroupProfFunc = 1

[version2.8.6.4][DLL:2.8.6.0][ResImageMaker:2.0.1.0]: 2016/11/22
1. Check param file for V50. 

[version2.8.6.5][DLL:2.8.6.0][ResImageMaker:2.0.1.0]: 2017/04/07
1. Support FW1 Write-Protect.
2. Fix FW+PartRsv Readverify error(open an existed file fail).

[version2.8.6.6][DLL:2.8.6.1][ResImageMaker:2.0.1.0]: 2017/05/23
1. Support program all with IMG files trimed.(Set "TrimImageFunc = 1" in [FUNC_LIST] of profile)
2. Fix memory leak when card isp with backup list.

[version2.8.6.7][DLL:2.8.6.1][ResImageMaker:2.0.1.0]: 2017/05/25
1. Fix the error of comparing AIMG at trim mode.

[version2.8.6.8][DLL:2.8.6.1][ResImageMaker:2.0.1.0]: 2017/08/08
1. Support setting "BackupSilence=1" in [CURRENT] of frm_user.ini file to disable the dialog of "File Replace" when run ProgramAll with Backup/Restore RSV.
