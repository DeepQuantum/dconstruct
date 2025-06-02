#include "listing_view_controller.h"


struct DCStruct {
    virtual void parse() = 0;
    virtual void insert(ListingViewController *lvc) = 0;
};

struct DCArray: public DCStruct {
    void parse() override {

    }

    void insert(ListingViewController *lvc) override {
        
    }
};

struct  dcmap : public DCStruct {
    u32 size;
    DCArray keys;
    DCArray values_offset;

    void parse() override {

    }

    void insert(ListingViewController *lvc) override {
        this->insertSpan("\n");
        this->insertSpan("SIZE: " + std::to_string(size), MainWindow::STRING_COLOR, 12, 6);

        this->insertSpan("\n");
    }
};

void ListingViewController::parseMap(const Symbol &symbol) noexcept {
    u8 *start = reinterpret_cast<u8*>(symbol.raw_entry.m_entryPtr);
    dcmap map = *reinterpret_cast<dcmap*>(start);

    
}