#include <string.h>
#include <stdio.h>


#define PAGE_CAPACITY 1 << 8
#define FRAME_SIZE 1 << 8
#define TLB_CAPACITY 16

struct page {
    int offset;
    int n;
    int frame_n;
};

FILE *backing_store;

struct page tlb [TLB_CAPACITY]; // массив структур page для TLB
struct page pages [PAGE_CAPACITY]; // массив структур page для хранения загруженных страниц
char ram[PAGE_CAPACITY][FRAME_SIZE]; // массив для хранения фреймов

char tlb_len = 0;
int frames_len = 0;
int pages_len = 0;
int processed_pages = 0; // счётчик обработанных страниц

const int offset_mask = (1 << 8) - 1;

int page_fault_cnt = 0; // счётчик ошибок
int tlb_hit = 0; // счётчик попаданий в TLB

// Загрузка данных страницы из файла в память, обновление информации о странице.
int read_from_file(struct page *curr_page) {
    if (frames_len >= PAGE_CAPACITY || pages_len >= PAGE_CAPACITY) {
        return -1;
    }

    fseek(backing_store, curr_page->n * FRAME_SIZE, SEEK_SET);
    fread(ram[frames_len], sizeof(char), FRAME_SIZE, backing_store);

    pages[pages_len].n = curr_page->n;
    pages[pages_len].frame_n = frames_len;
    pages_len++;

    return frames_len++;
}

// получает структуру page из логического адреса.
struct page get_page(int log_address) {
    struct page curr_page;
    curr_page.offset = log_address & offset_mask;
    curr_page.n = (log_address >> 8) & offset_mask;
    return curr_page;
}

// вставляет страницу в TLB, обрабатывает замещение при переполнении.
void insert_tlb(struct page *curr_page) {
    if (tlb_len == 16) {
        memmove(&tlb[0], &tlb[1], sizeof(struct page) * 15);
        tlb_len--;
    }
    tlb[tlb_len++] = *curr_page;
}

// получает фрейм для заданной страницы, обрабатывает TLB и страницы.
int get_frame(struct page *curr_page) {
    char hit = 0;

    for (int i = tlb_len; i >= 0; --i) {
        if (tlb[i].n == curr_page->n) {
            curr_page->frame_n = tlb[i].frame_n;
            ++tlb_hit;
            hit = 1;
            break;
        }
    }

    for (int i = 0; i < pages_len && !hit; ++i) {
        if (pages[i].n == curr_page->n) {
            curr_page->frame_n = pages[i].frame_n;
            hit = 1;
            break;
        }
    }

    if (!hit) {
        curr_page->frame_n = read_from_file(curr_page);
        ++page_fault_cnt;
    }

    insert_tlb(curr_page);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2 || !freopen(argv[1], "r", stdin)) {
        printf("input reading error\n");
        return 1;
    }

    backing_store = fopen("BACKING_STORE.bin", "rb");
    if (backing_store == NULL) {
        printf("BACKING_STORE.bin error\n");
        return 1;
    }

    freopen("check.txt", "w", stdout);

    int log_address = 0;

    while (scanf("%d", &log_address) == 1) {
        struct page curr_page = get_page(log_address);
        get_frame(&curr_page);
        printf("Virtual address: %d Physical address: %d Value: %d\n", (curr_page.n << 8) | curr_page.offset, (curr_page.frame_n << 8) | curr_page.offset, ram[curr_page.frame_n][curr_page.offset]);
        ++processed_pages;
    }

    printf("\nPage fault frequency = %0.2f%%\n", page_fault_cnt * 100. / processed_pages);
    printf("TLB frequency = %0.2f%%\n", tlb_hit * 100. / processed_pages);

    return 0;
}