#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// structure definition
struct Product {
    int productId;
    char productName[50];
    float price;
    int quantity;
};

// function prototypes
void addProduct(struct Product **inventory, int *size);
void viewProducts(struct Product *inventory, int size);
void updateQuantity(struct Product *inventory, int size);
void searchById(struct Product *inventory, int size);
void searchByName(struct Product *inventory, int size);
void searchByPriceRange(struct Product *inventory, int size);
void deleteProduct(struct Product **inventory, int *size);
void freeMemory(struct Product *inventory);

// validation helper functions
int validatePositiveInt(int value);
int validateRangeInt(int value, int min, int max);
int validateRangeFloat(float value, float min, float max);
int validateName(char *name);
int isUniqueId(struct Product *inventory, int size, int id);

int main() {
    struct Product *inventory = NULL;
    int size, choice;

    // taking valid number of products
    do {
        printf("Enter initial number of products (1-100): ");
        if (scanf("%d", &size) != 1 || !validateRangeInt(size, 1, 100)) {
            printf("Invalid input,re-enter valid number.\n");
            while (getchar() != '\n');
            size = 0;
        }
    } while (size == 0);

    // allocate memory using calloc
    inventory = (struct Product *)calloc(size, sizeof(struct Product));

    // take input for product details
    for (int i = 0; i < size; i++) {
        printf("\nEnter details for product %d:\n", i + 1);

        // Product ID validation (unique)
        do {
            printf("Product ID (1-10000): ");
            if (scanf("%d", &inventory[i].productId) != 1 ||
                !validateRangeInt(inventory[i].productId, 1, 10000) ||
                !isUniqueId(inventory, i, inventory[i].productId)) {
                printf("Invalid or duplicate Product ID,re-enter valid ID.\n");
                while (getchar() != '\n');
                inventory[i].productId = 0;
            }
        } while (inventory[i].productId == 0);

        // Product Name validation
        do {
            printf("Product Name (only letters, 1-50 chars): ");
            scanf(" %[^\n]", inventory[i].productName);
            if (!validateName(inventory[i].productName)) {
                printf("Invalid name, Only alphabets are allowed.\n");
                strcpy(inventory[i].productName, "");
            }
        } while (strlen(inventory[i].productName) == 0);

        // Product Price validation
        do {
            printf("Product Price (0-100000): ");
            if (scanf("%f", &inventory[i].price) != 1 ||
                !validateRangeFloat(inventory[i].price, 0, 100000)) {
                printf("Invalid Price,re-enter valid price.\n");
                while (getchar() != '\n');
                inventory[i].price = -1;
            }
        } while (inventory[i].price < 0);

        // Product Quantity validation
        do {
            printf("Product Quantity (0-1000000): ");
            if (scanf("%d", &inventory[i].quantity) != 1 ||
                !validateRangeInt(inventory[i].quantity, 0, 1000000)) {
                printf("Invalid Quantity.\n");
                while (getchar() != '\n');
                inventory[i].quantity = -1;
            }
        } while (inventory[i].quantity < 0);
    }

    // Inventory menu for user choices
    do {
        printf("\n========= INVENTORY MENU =========\n");
        printf("1. Add New Product\n");
        printf("2. View All Products\n");
        printf("3. Update Quantity\n");
        printf("4. Search Product by ID\n");
        printf("5. Search Product by Name\n");
        printf("6. Search Product by Price Range\n");
        printf("7. Delete Product\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");

        do {
            if (scanf("%d", &choice) != 1) {
                printf("Invalid choice, enter a number between 1 and 8 :");
                while (getchar() != '\n');
                choice = 0;
            } else break;
        } while (1);

        switch (choice) {
            case 1:
                addProduct(&inventory, &size);
                break;
            case 2:
                viewProducts(inventory, size);
                break;
            case 3:
                updateQuantity(inventory, size);
                break;
            case 4:
                searchById(inventory, size);
                break;
            case 5:
                searchByName(inventory, size);
                break;
            case 6:
                searchByPriceRange(inventory, size);
                break;
            case 7:
                deleteProduct(&inventory, &size);
                break;
            case 8:
                freeMemory(inventory);
                printf("Memory released successfully. Exiting program...\n");
                break;
            default:
                printf("Invalid choice, Please try again.\n");
        }
    } while (choice != 8);

    return 0;
}

// function to add a new product
void addProduct(struct Product **inventory, int *size) {
    if (*size >= 100) {
        printf("Cannot add products, Maximum product limit reached.\n");
        return;
    }

    (*size)++;
    *inventory = (struct Product *)realloc(*inventory, (*size) * sizeof(struct Product));

    struct Product *newProduct = &((*inventory)[*size - 1]);

    // Product ID validation (unique)
    do {
        printf("Product ID (1-10000): ");
        if (scanf("%d", &newProduct->productId) != 1 ||
            !validateRangeInt(newProduct->productId, 1, 10000) ||
            !isUniqueId(*inventory, *size - 1, newProduct->productId)) {
            printf("Invalid or duplicate Product ID,re-enter valid ID.\n");
            while (getchar() != '\n');
            newProduct->productId = 0;
        }
    } while (newProduct->productId == 0);

    // Product Name validation
    do {
        printf("Product Name (only letters, 1-50 chars): ");
        scanf(" %[^\n]", newProduct->productName);
        if (!validateName(newProduct->productName)) {
            printf("Invalid name, Only alphabets are allowed.\n");
            strcpy(newProduct->productName, "");
        }
    } while (strlen(newProduct->productName) == 0);

    // Product Price validation
    do {
        printf("Product Price (0-100000): ");
        if (scanf("%f", &newProduct->price) != 1 ||
            !validateRangeFloat(newProduct->price, 0, 100000)) {
            printf("Invalid Price.\n");
            while (getchar() != '\n');
            newProduct->price = -1;
        }
    } while (newProduct->price < 0);

    // Product Quantity validation
    do {
        printf("Product Quantity (0-1000000): ");
        if (scanf("%d", &newProduct->quantity) != 1 ||
            !validateRangeInt(newProduct->quantity, 0, 1000000)) {
            printf("Invalid Quantity.\n");
            while (getchar() != '\n');
            newProduct->quantity = -1;
        }
    } while (newProduct->quantity < 0);

    printf("Product added successfully!\n");
}

// function to view all products
void viewProducts(struct Product *inventory, int size) {
    if (size == 0) {
        printf("No products available.\n");
        return;
    }

    printf("======= PRODUCT LIST =======\n");
    for (int i = 0; i < size; i++) {
        printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
               inventory[i].productId, inventory[i].productName,
               inventory[i].price, inventory[i].quantity);
    }
}

// update quantity with validation
void updateQuantity(struct Product *inventory, int size) {
    int id, newQuantity, found = 0;
    do {
        printf("Enter Product ID to update quantity: ");
        if (scanf("%d", &id) != 1 || !validateRangeInt(id, 1, 10000)) {
            printf("Invalid Product ID!\n");
            while (getchar() != '\n');
            id = -1;
        }
    } while (id < 0);

    for (int i = 0; i < size; i++) {
        if (inventory[i].productId == id) {
            do {
                printf("Enter new Quantity (0-1000000): ");
                if (scanf("%d", &newQuantity) != 1 ||
                    !validateRangeInt(newQuantity, 0, 1000000)) {
                    printf("Invalid quantity!\n");
                    while (getchar() != '\n');
                    newQuantity = -1;
                }
            } while (newQuantity < 0);
            inventory[i].quantity = newQuantity;
            printf("Quantity updated successfully!\n");
            found = 1;
            break;
        }
    }
    if (!found)
        printf("Product ID not found!\n");
}

// search by ID with validation
void searchById(struct Product *inventory, int size) {
    int id, found = 0;
    do {
        printf("Enter Product ID to search: ");
        if (scanf("%d", &id) != 1 || !validateRangeInt(id, 1, 10000)) {
            printf("Invalid Product ID!\n");
            while (getchar() != '\n');
            id = -1;
        }
    } while (id < 0);

    for (int i = 0; i < size; i++) {
        if (inventory[i].productId == id) {
            printf("Product Found: Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   inventory[i].productId, inventory[i].productName,
                   inventory[i].price, inventory[i].quantity);
            found = 1;
            break;
        }
    }
    if (!found)
        printf("Product not found!\n");
}

// search by name with validation
void searchByName(struct Product *inventory, int size) {
    char name[50];
    int found = 0;

    do {
        printf("Enter name to search : ");
        scanf(" %[^\n]", name);
        if (!validateName(name)) {
            printf("Invalid name! Only alphabets are allowed.\n");
            strcpy(name, "");
        }
    } while (strlen(name) == 0);

    printf("Products Found:\n");
    for (int i = 0; i < size; i++) {
        if (strstr(inventory[i].productName, name) != NULL) {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   inventory[i].productId, inventory[i].productName,
                   inventory[i].price, inventory[i].quantity);
            found = 1;
        }
    }
    if (!found)
        printf("No products match the given name.\n");
}

// search by price range with validation
void searchByPriceRange(struct Product *inventory, int size) {
    float minPrice, maxPrice;
    int found = 0;

    // validate minimum price
    do {
        printf("Enter minimum price (0-100000): ");
        if (scanf("%f", &minPrice) != 1 ||
            !validateRangeFloat(minPrice, 0, 100000)) {
            printf("Invalid minimum price!\n");
            while (getchar() != '\n');
            minPrice = -1;
        }
    } while (minPrice < 0);

    // validate maximum price
    do {
        printf("Enter maximum price (0-100000): ");
        if (scanf("%f", &maxPrice) != 1 ||
            !validateRangeFloat(maxPrice, 0, 100000) ||
            maxPrice < minPrice) {
            printf("Invalid maximum price!\n");
            while (getchar() != '\n');
            maxPrice = -1;
        }
    } while (maxPrice < 0);

    printf("Products in price range:\n");
    for (int i = 0; i < size; i++) {
        if (inventory[i].price >= minPrice && inventory[i].price <= maxPrice) {
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   inventory[i].productId, inventory[i].productName,
                   inventory[i].price, inventory[i].quantity);
            found = 1;
        }
    }
    if (!found)
        printf("No products found in this price range.\n");
}

// delete product with validation
void deleteProduct(struct Product **inventory, int *size) {
    int id, index = -1;

    do {
        printf("Enter Product ID to delete: ");
        if (scanf("%d", &id) != 1 || !validateRangeInt(id, 1, 10000)) {
            printf("Invalid Product ID!\n");
            while (getchar() != '\n');
            id = -1;
        }
    } while (id < 0);

    for (int i = 0; i < *size; i++) {
        if ((*inventory)[i].productId == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Product ID not found!\n");
        return;
    }

    for (int i = index; i < *size - 1; i++) {
        (*inventory)[i] = (*inventory)[i + 1];
    }

    (*size)--;
    *inventory = (struct Product *)realloc(*inventory, (*size) * sizeof(struct Product));

    printf("Product deleted successfully!\n");
}

// free memory
void freeMemory(struct Product *inventory) {
    free(inventory);
}

// validation functions
int validatePositiveInt(int value) {
    return value > 0;
}

int validateRangeInt(int value, int min, int max) {
    return value >= min && value <= max;
}

int validateRangeFloat(float value, float min, float max) {
    return value >= min && value <= max;
}

int validateName(char *name) {
    for (int i = 0; i < strlen(name); i++) {
        if (!isalpha(name[i]) && name[i] != ' ')
            return 0;
    }
    return strlen(name) > 0 && strlen(name) <= 50;
}

// function to ensure Product ID is unique
int isUniqueId(struct Product *inventory, int size, int id) {
    for (int i = 0; i < size; i++) {
        if (inventory[i].productId == id)
            return 0;
    }
    return 1;
}
