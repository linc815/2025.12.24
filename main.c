#define _CRT_SECURE_NO_WARNINGS 
#include<stdio.h>
#include<string.h>
#define MAX_LOGS 100
#define MAX_LOG_LEN 1024


typedef struct {
	char content[MAX_LOG_LEN];//logs[i].content 意思是第i条日志的内容
} Log;
typedef struct //用index 来锁定有哪些log里有关键字的内容
{
	char word[32];
	int log_ids[100];
	int count;
}IndexEntry;
typedef struct
{
	int log_ids[MAX_LOGS];
	int count;
}IndexResult;
//int readLogLine(char* out, int out_size)
//{
//	if (fgets(out, out_size, stdin) == NULL)
//	{
//		return 0;
//	}
//	int len = strlen(out);
//	if (len > 0 && out[len - 1] == '\n')
//	{
//		return 1;
//	}
//	
//
//
//}

void readBlockInput(char* content, int size)
{
	char line[256];

	int pos = 0;
	content[0] = '\0';
	while (1)
	{
		if (fgets(line, sizeof(line), stdin) == NULL)
		{
			return;
		}
		if (line[0] == '\n')
		{
			break;
		}
		int i = 0;
		while (line[i] != '\0' && pos < size - 1)
		{
			content[pos] = line[i];
			pos++;
			i++;
		}
		content[pos] = '\0';
	}
}
void appendLog()
{
	FILE* fp = fopen("data.txt", "a");
	char content[256];
	if (fp == NULL)
	{
		return ;
	}
	printf("enter record:\n");
	/*if (!readLogLine(content, sizeof(content)))
	{
		fclose(fp);
		return;
	}*/
	readBlockInput(content, sizeof(content));
	fprintf(fp, "%s\n\n", content);
	fclose(fp);
}
void showLogs()
{
	FILE* fp = fopen("data.txt","r");
	char line[256];
	if (fp == NULL) {
		printf("File open failed.\n");
		return ;
	}
	while (fgets(line, sizeof(line), fp))
	{


		printf("%s", line);
	}
	fclose(fp);
}

int loadLogs(Log logs[], int max_logs)
{
	
	
		FILE* fp = fopen("data.txt", "r");
		char line[256];
		char content[MAX_LOG_LEN];
		int pos = 0;
		int count = 0;

		content[0] = '\0';

		while (fgets(line, sizeof(line), fp) != NULL)
		{
			if (line[0] == '\n')
			{
				if (count < max_logs && pos > 0)
				{
					strcpy(logs[count].content, content);
					count++;
					pos = 0;
					content[0] = '\0';
				}
			}
			else
			{
				int i = 0;
				while (pos < MAX_LOG_LEN - 1 && line[i] != '\0')
				{
					content[pos] = line[i];
					pos++;
					i++;
				}
				content[pos] = '\0';
			}
		}

		// EOF 后处理最后一个 block 防止用户最后没打空行
		if (pos > 0 && count < max_logs)
		{
			strcpy(logs[count].content, content);
			count++;
		}

		fclose(fp);
		return count;

}
IndexResult buildIndex(Log logs[], int log_count, const char* keyword)
{
	IndexResult result;
	result.count = 0;
	for (int i = 0;i < log_count;i++)
	{
		if (strstr(logs[i].content, keyword) != NULL)
		{
			result.log_ids[result.count] = i;
			result.count++;
		}
	}
	return result;
}
void searchLogs(Log logs[], int log_count, const char* keyword)
{
	IndexResult idx = buildIndex(logs, log_count, keyword);
	for (int k = 0;k < idx.count;k++)
	{
		int i = idx.log_ids[k];
		printf("----Log %d ----\n", i);
		printf("%s\n", logs[i].content);
		
	}
}
/*
Edit via temp template:

- Block-based text file (blocks separated by empty line)
- Read original file line by line
- Decide whether to edit at block boundary
- Write new content to temp file
- Replace original file atomically

Decision logic is injected via shouldEditBlock().
*/
int shouldEditBlock(int current_id, int target_id)
{
	return current_id == target_id;
}
int buildFinalBlock(
	int current_id,
	int target_id,
	const char* old_block,
	char* out_block,
	int out_size
)
{
	if (current_id == target_id)
	{
		readBlockInput(out_block, out_size);
		return 1;   // 使用新 block
	}

	// 不改：把旧 block 拷贝到 out_block
	strncpy(out_block, old_block, out_size - 1);
	out_block[out_size - 1] = '\0';
	return 1;
}
void editLogById(int target_id)
{
	FILE* fp = fopen("data.txt", "r");
	FILE* tem = fopen("temp.txt", "w");
	int current_id = 0;
	char line[256];
	char block[256];
	int pos = 0;
	block[0] = '\0';
	if (fp == NULL || tem == NULL)
	{
		return;
	}
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		if (line[0] == '\n')
		{
			if (shouldEditBlock(current_id,target_id))
			{
				char final_block[256];

				buildFinalBlock(
					current_id,
					target_id,
					block,
					final_block,
					sizeof(final_block)
				);

				fprintf(tem, "%s\n", final_block);
			}
			else
			{
				fprintf(tem, "%s\n", block);
			}
			current_id++;
			pos = 0;
			block[0] = '\0';
		}
		else
		{
			
			
			
			
				int i = 0;
				while (line[i] != '\0' && pos < sizeof(block) - 1)
				{
					block[pos] = line[i];
					pos++;
					i++;
				}
				block[pos] = '\0';
			
			
		}

	}
	fclose(fp);
	fclose(tem);
	remove("data.txt");
	rename("temp.txt", "data.txt");
}
//void editLog(int target_id, const char* new_content)
//{
//	
//}
//


		





int main()
{
	Log logs[MAX_LOGS];   // ← 必须有
	int count = loadLogs(logs, MAX_LOGS);

	char keyword[256];
	printf("Enter keyword:\n");
	fgets(keyword, sizeof(keyword), stdin);
	keyword[strcspn(keyword, "\n")] = '\0';

	searchLogs(logs, count, keyword);

	appendLog();
	showLogs();

	return 0;
}