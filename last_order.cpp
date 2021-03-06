#include "volume_information.h"
#include "sister_allocation_table.h"
#include "root_directory.h"
#include "data_pool.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

#include "params.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>

using namespace std;

VolumeInformation vi;
SisterAllocationTable sat;
RootDirectory rd;
vector <DataPool> dp;

void readFile(char* file){
	dp.clear();
	string line;
	string whole = "";
  	ifstream myfile (file);
  	if (myfile.is_open())
  	{
    	while ( getline (myfile,line) )
    	{
      		whole = whole + line;
    	}
    	myfile.close();
    	//cout << whole.length() << endl;
	  	// Volume Information
	  	string svi = "";
	  	for (int i=0;i<1024;i++){
			svi = svi + whole[i];
		}
		vi.load(svi);
		//cout << svi.length() << endl;
		//cout << svi << endl;
	  	// Sister Allocation Table
		string ssat = "";
		int offset = 1024;
	  	for (int i=0;i<128*1024;i++){
			ssat = ssat + whole[offset+i];
		}
		sat.load(ssat);
		//cout << ssat.length() << endl;
		//cout << ssat << endl;
	  	// Root Directory
		string srd = "";
		offset+= 128*1024;
	  	for (int i=0;i<1024;i++){
			srd = srd + whole[offset+i];
		}
		rd.load(srd);
		//cout << srd.length() << endl;
		//cout << srd << endl;
	  	// Data Pool
		offset+=1024;
		int idxx = 0;
		while (offset < whole.length()){
		  	string sdp = "";
		  	for (int i=offset;i<whole.length();i++){
				sdp = sdp + whole[i];
			}
			DataPool dpt;
			dpt.load(sdp);
			dp.push_back(dpt);
			//cout << sdp.length() << endl;
			//cout << idxx << " " << sdp << endl;
			idxx++;
			offset+=32;
		}
		cout << whole.length() << endl;
	}
  	else 
  		cout << "Unable to open file"; 
}

void writeFile(char* file){
	string whole;
	ofstream myfile (file);
	if (myfile.is_open()){
		//cout << "write1" << endl;
		whole = whole + vi.toString();
		//cout << vi.toString().length() << endl;
		//cout << "write2" << endl;		
		whole = whole + sat.toString();
		//cout << sat.toString().length() << endl;
		//cout << "write3" << endl;
		whole = whole + rd.toString();
		//cout << rd.toString().length() << endl;
		//cout << "write4" << endl;
		for (int i=0;i<dp.size();i++){
			//cout << "write5" << i << endl;
			whole = whole + dp[i].toString();
			//cout << dp[i].toString().length() << endl;
		}
		cout << whole.length() << endl;
		//cout << endl;
		//cout << endl;
		myfile << whole;
		myfile.close();
	}
	else 
		cout << "Unable to open file";
}

void generateFile(string namaFile){

}

// PART OF BIG BRO
int bb_getattr(const char *path, struct stat *statbuf)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_getattr(path=\"%s\", statbuf=0x%08x)\n",
	  path, statbuf);
    bb_fullpath(fpath, path);
    
    retstat = lstat(fpath, statbuf);
    if (retstat != 0)
	retstat = bb_error("bb_getattr lstat");
    
    log_stat(statbuf);
    
    return retstat;
}
int bb_mknod(const char *path, mode_t mode, dev_t dev)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_mknod(path=\"%s\", mode=0%3o, dev=%lld)\n",
	  path, mode, dev);
    bb_fullpath(fpath, path);
    
    // On Linux this could just be 'mknod(path, mode, rdev)' but this
    //  is more portable
    if (S_ISREG(mode)) {
        retstat = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
	if (retstat < 0)
	    retstat = bb_error("bb_mknod open");
        else {
            retstat = close(retstat);
	    if (retstat < 0)
		retstat = bb_error("bb_mknod close");
	}
    } else
	if (S_ISFIFO(mode)) {
	    retstat = mkfifo(fpath, mode);
	    if (retstat < 0)
		retstat = bb_error("bb_mknod mkfifo");
	} else {
	    retstat = mknod(fpath, mode, dev);
	    if (retstat < 0)
		retstat = bb_error("bb_mknod mknod");
	}
    
    return retstat;
}

int bb_mkdir(const char *path, mode_t mode)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_mkdir(path=\"%s\", mode=0%3o)\n",
	    path, mode);
    bb_fullpath(fpath, path);
    
    retstat = mkdir(fpath, mode);
    if (retstat < 0)
	retstat = bb_error("bb_mkdir mkdir");
    
    return retstat;
}

int bb_rmdir(const char *path)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("bb_rmdir(path=\"%s\")\n",
	    path);
    bb_fullpath(fpath, path);
    
    retstat = rmdir(fpath);
    if (retstat < 0)
	retstat = bb_error("bb_rmdir rmdir");
    
    return retstat;
}

int bb_rename(const char *path, const char *newpath)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    char fnewpath[PATH_MAX];
    
    log_msg("\nbb_rename(fpath=\"%s\", newpath=\"%s\")\n",
	    path, newpath);
    bb_fullpath(fpath, path);
    bb_fullpath(fnewpath, newpath);
    
    retstat = rename(fpath, fnewpath);
    if (retstat < 0)
	retstat = bb_error("bb_rename rename");
    
    return retstat;
}

int bb_truncate(const char *path, off_t newsize)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_truncate(path=\"%s\", newsize=%lld)\n",
	    path, newsize);
    bb_fullpath(fpath, path);
    
    retstat = truncate(fpath, newsize);
    if (retstat < 0)
	bb_error("bb_truncate truncate");
    
    return retstat;
}
int bb_open(const char *path, struct fuse_file_info *fi)
{
    int retstat = 0;
    int fd;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_open(path\"%s\", fi=0x%08x)\n",
	    path, fi);
    bb_fullpath(fpath, path);
    
    fd = open(fpath, fi->flags);
    if (fd < 0)
	retstat = bb_error("bb_open open");
    
    fi->fh = fd;
    log_fi(fi);
    
    return retstat;
}
int bb_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nbb_read(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
	    path, buf, size, offset, fi);
    // no need to get fpath on this one, since I work from fi->fh not the path
    log_fi(fi);
    
    retstat = pread(fi->fh, buf, size, offset);
    if (retstat < 0)
	retstat = bb_error("bb_read read");
    
    return retstat;
}
int bb_write(const char *path, const char *buf, size_t size, off_t offset,
	     struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nbb_write(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
	    path, buf, size, offset, fi
	    );
    // no need to get fpath on this one, since I work from fi->fh not the path
    log_fi(fi);
	
    retstat = pwrite(fi->fh, buf, size, offset);
    if (retstat < 0)
	retstat = bb_error("bb_write pwrite");
    
    return retstat;
}

struct fuse_operations bb_oper = {
  .getattr = bb_getattr,
  .readlink = NULL,
  // no .getdir -- that's deprecated
  .getdir = NULL,
  .mknod = bb_mknod,
  .mkdir = bb_mkdir,
  .unlink = NULL,
  .rmdir = bb_rmdir,
  .symlink = NULL,
  .rename = bb_rename,
  .link = NULL,
  .chmod = NULL,
  .chown = NULL,
  .truncate = bb_truncate,
  .utime = NULL,
  .open = bb_open,
  .read = bb_read,
  .write = bb_write,
  /** Just a placeholder, don't set */ // huh???
  .statfs = NULL,
  .flush = NULL,
  .release = NULL,
  .fsync = NULL,
  .setxattr = NULL,
  .getxattr = NULL,
  .listxattr = NULL,
  .removexattr = NULL,
  .opendir = NULL,
  .readdir = bb_readdir,
  .releasedir = NULL,
  .fsyncdir = NULL,
  .init = NULL,
  .destroy = NULL,
  .access = NULL,
  .create = NULL,
  .ftruncate = NULL,
  .fgetattr = NULL
};

// ./LastOrder /mnt/hd/sisterfs sister.fs
// ./LastOrder /mnt/hd/sisterfs sister.fs shift

int main(int argc, char *argv[]){
	DataPool dpt;

	dp.push_back(dpt);
	writeFile("sister.fs");
	readFile("sister.fs");
	
	if (argc < 3 || argc > 4) {
		cout << "Shift failed, invalid parameter[s]" << endl;
	}
	else if (argc == 3){
		cout << "Shift success" << endl;
		cout << argv[2] << "c" << endl;
		readFile(argv[2]);
	}
	else if (argc == 4){
		if (strcmp(argv[3],"shift") == 0){
			cout << "Shift success" << endl;
			string str = argv[1];
			//generateFile(str);
		}
		else {
			cout << "Shift failed, invalid parameter[s]" << endl;
		}
	}
}
