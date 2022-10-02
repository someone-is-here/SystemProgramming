#pragma once

namespace SP1 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form {
	public:
		MyForm(void) {
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm() {
			if (components) {
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ button1;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::ColorDialog^ colorDialog1;
	private: System::Windows::Forms::Button^ button2;
	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void) {
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->colorDialog1 = (gcnew System::Windows::Forms::ColorDialog());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->BackColor = System::Drawing::SystemColors::MenuHighlight;
			this->button1->Location = System::Drawing::Point(89, 174);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(93, 34);
			this->button1->TabIndex = 0;
			this->button1->Text = L"ClickMe";
			this->button1->UseVisualStyleBackColor = false;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// label1
			// 
			this->label1->AccessibleName = L"Result";
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 36, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label1->Location = System::Drawing::Point(106, 89);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(63, 69);
			this->label1->TabIndex = 1;
			this->label1->Text = L"0";
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(-6, -4);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(132, 37);
			this->button2->TabIndex = 2;
			this->button2->Text = L"ChangeColor";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(282, 292);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button1);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
		label1->Text = System::Convert::ToString(System::Convert::ToInt64(label1->Text) + 1);
	}
	private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {
		if (colorDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			button1->BackColor = colorDialog1->Color;
		}
	};
	private: System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e){
		this->colorDialog1->Color = Color::Green;
	}
	};
}