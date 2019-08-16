#ifndef _UTILS_H
#define _UTILS_H

/**
 * @brief 分割字符串，以空格为分隔符
 *
 * @param in_str 待分割的字符串
 * @param out_str 去掉命令后的第一个参数
 *
 * @return 0成功，-1失败
 */
// get, 支持
// get file, 支持
// get file1 file2, 不支持
int split_string2(char *in_str, char *out_str);

/**
 * @brief 获取文件的实际长度
 *
 * @param filename 文件名
 *
 * @return >=0成功，-1失败
 */
long get_length(char *filename);

/**
 * @brief 获取文件的md5
 *
 * @param filename
 * @param md5sum
 */
void get_md5(char *filename, char *md5sum);

#endif // UTILS_H
